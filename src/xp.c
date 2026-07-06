// xp.c — gemas de XP: drop, ímã e coleta.
//
// Design: estado estático encapsulado no módulo (mesmo padrão de enemy.c/
// world.c). Um pool fixo de gemas (array de MAX_GEMS) evita alocação dinâmica
// no loop; cada slot tem um flag `active`. XpSpawn recicla slots inativos.
// O total de XP acumulado é mantido internamente e exposto por XpGetTotal.
#include "xp.h"

#include "raymath.h"

// --- Pool ---
#define MAX_GEMS         500    // capacidade máxima do pool de gemas

// --- Parâmetros ---
#define GEM_XP_VALUE     1      // XP concedido por gema coletada
#define GEM_PICKUP_RADIUS 18.0f // distância player↔gema pra coletar (px)
#define GEM_MAGNET_RADIUS 80.0f // distância inicial a partir da qual a gema é atraída (px)
#define GEM_MAGNET_SPEED  260.0f // velocidade de atração no ímã (px/s)

// --- Desenho ---
#define GEM_RADIUS       6.0f   // "raio" do losango desenhado (px)
#define GEM_SIDES        4      // losango (polígono de 4 lados)
#define GEM_ROTATION     45.0f  // gira o quadrado 45° pra virar losango

// Uma gema do pool: posição no mundo e flag de ativo.
typedef struct Gem {
    Vector2 position; // posição no mundo (centro)
    bool    active;   // slot ocupado?
} Gem;

// Estado do módulo (estático, encapsulado — mesmo padrão de enemy.c/world.c).
static Gem   gems[MAX_GEMS];
static int   totalXp;      // XP acumulado (soma das gemas coletadas)
static float magnetRadius; // raio de atração atual (px); aumentado por upgrades

void XpInit(void)
{
    for (int i = 0; i < MAX_GEMS; i++)
        gems[i].active = false;

    totalXp      = 0;
    magnetRadius = GEM_MAGNET_RADIUS;
}

void XpSpawn(Vector2 pos)
{
    for (int i = 0; i < MAX_GEMS; i++)
    {
        if (gems[i].active) continue;

        gems[i].position = pos;
        gems[i].active   = true;
        return;
    }
    // Pool cheio: nenhum slot livre. Ignora este drop.
}

void XpUpdate(float dt, Player *player)
{
    const float pickup2  = GEM_PICKUP_RADIUS * GEM_PICKUP_RADIUS;
    const float magnet2  = magnetRadius * magnetRadius;

    for (int i = 0; i < MAX_GEMS; i++)
    {
        Gem *g = &gems[i];
        if (!g->active) continue;

        float d2 = Vector2DistanceSqr(player->position, g->position);

        // --- Coleta: encostou no player. ---
        if (d2 < pickup2)
        {
            g->active = false;
            totalXp  += GEM_XP_VALUE;
            continue;
        }

        // --- Ímã: dentro do raio de atração, move-se em direção ao player. ---
        if (d2 < magnet2)
        {
            Vector2 toPlayer = Vector2Subtract(player->position, g->position);
            if (Vector2LengthSqr(toPlayer) > 0.0001f)
            {
                Vector2 dir = Vector2Normalize(toPlayer);
                g->position = Vector2Add(g->position,
                                         Vector2Scale(dir, GEM_MAGNET_SPEED * dt));
            }
        }
    }
}

void XpDraw(void)
{
    for (int i = 0; i < MAX_GEMS; i++)
    {
        const Gem *g = &gems[i];
        if (!g->active) continue;

        // Losango ciano preenchido + contorno mais claro pra destacar do chão.
        DrawPoly(g->position, GEM_SIDES, GEM_RADIUS, GEM_ROTATION, SKYBLUE);
        DrawPolyLines(g->position, GEM_SIDES, GEM_RADIUS, GEM_ROTATION, WHITE);
    }
}

int XpGetTotal(void)
{
    return totalXp;
}

void XpUpgradeMagnet(float factor)
{
    // Aumenta o raio de atração das gemas (factor > 1 => atrai de mais longe).
    magnetRadius *= factor;
}
