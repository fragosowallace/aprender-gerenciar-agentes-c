// enemy.c — implementação dos inimigos (slimes roxos).
//
// Design: estado estático no módulo (mesmo padrão do world.c). Um pool fixo de
// inimigos (array de MAX_ENEMIES) evita alocação dinâmica no loop; cada slot tem
// um flag `active`. O spawn recicla slots inativos. Um contador de tempo interno
// dispara o spawn periódico.
#include "enemy.h"

#include "raymath.h"
#include "config.h"

// --- Sprite do inimigo ---
// Spritesheet: strip horizontal de 4 frames de 16x16 (64x16), ciclo de pulo.
// O `source` seleciona x = frame*16, y = 0.
#define ENEMY_FRAME_W    16    // largura de cada frame no spritesheet (px)
#define ENEMY_FRAME_H    16    // altura de cada frame (px)
#define ENEMY_SCALE      2     // cada pixel do sprite = 2px na tela (escala inteira)
#define ENEMY_FRAMES     4     // frames no ciclo de animação
#define ENEMY_FRAME_TIME 0.15f // tempo por frame do ciclo de pulo (s)

// --- Pool ---
#define MAX_ENEMIES      300   // capacidade máxima do pool

// --- Comportamento ---
#define ENEMY_SPEED      110.0f // px/s (< speed do player, ~250)
#define ENEMY_DAMAGE     10.0f  // dano por contato aplicado ao player
#define ENEMY_RADIUS     14.0f  // raio de colisão do inimigo (px, no mundo)
#define PLAYER_RADIUS    16.0f  // raio de colisão do player (px, no mundo)

// --- Spawn ---
#define SPAWN_INTERVAL   0.8f   // intervalo entre spawns (s)
#define SPAWN_RADIUS     520.0f // distância do player onde o inimigo surge (fora da tela)

// Um inimigo do pool: posição no mundo, flag de ativo e estado de animação.
typedef struct Enemy {
    Vector2 position; // posição no mundo (centro do sprite)
    bool    active;   // slot ocupado por um inimigo vivo?
    int     frame;    // frame atual do ciclo de animação
    float   timer;    // acumulador de tempo pra troca de frame
} Enemy;

// Estado do módulo (estático, encapsulado — mesmo padrão do world.c).
static Enemy     enemies[MAX_ENEMIES];
static Texture2D enemyTex;
static float     spawnTimer; // acumula dt; ao passar de SPAWN_INTERVAL, spawna

// Ativa um slot livre do pool, posicionando o inimigo num ponto FORA da tela,
// num raio ao redor do player, em ângulo aleatório. Se o pool estiver cheio,
// simplesmente ignora (sem overflow).
static void SpawnEnemy(Vector2 playerPos)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active) continue;

        // Ângulo aleatório em [0, 2*PI). GetRandomValue dá inteiros; convertemos.
        float angle = (float)GetRandomValue(0, 359) * DEG2RAD;
        Vector2 offset = { cosf(angle) * SPAWN_RADIUS, sinf(angle) * SPAWN_RADIUS };

        enemies[i].position = Vector2Add(playerPos, offset);
        enemies[i].active   = true;
        enemies[i].frame    = GetRandomValue(0, ENEMY_FRAMES - 1); // dessincroniza
        enemies[i].timer    = 0.0f;
        return;
    }
    // Pool cheio: nenhum slot livre. Ignora o spawn deste tick.
}

void EnemyInit(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        enemies[i].active = false;

    spawnTimer = 0.0f;

    enemyTex = LoadTexture("assets/enemy.png");
    // Filtro POINT: mantém o pixel art nítido, sem borrar ao escalar.
    SetTextureFilter(enemyTex, TEXTURE_FILTER_POINT);
}

void EnemyUpdate(float dt, Player *player)
{
    // --- Spawn periódico ---
    spawnTimer += dt;
    while (spawnTimer >= SPAWN_INTERVAL)
    {
        spawnTimer -= SPAWN_INTERVAL;
        SpawnEnemy(player->position);
    }

    // Distância de contato: soma dos raios (comparada ao quadrado pra evitar sqrt).
    const float contactDist  = ENEMY_RADIUS + PLAYER_RADIUS;
    const float contactDist2 = contactDist * contactDist;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        // --- Perseguição: move na direção do player, independente de FPS. ---
        Vector2 toPlayer = Vector2Subtract(player->position, e->position);
        if (Vector2Length(toPlayer) > 0.0001f)
        {
            Vector2 dir = Vector2Normalize(toPlayer);
            e->position = Vector2Add(e->position,
                                     Vector2Scale(dir, ENEMY_SPEED * dt));
        }

        // --- Animação: cicla os 4 frames do pulo. ---
        e->timer += dt;
        if (e->timer >= ENEMY_FRAME_TIME)
        {
            e->timer -= ENEMY_FRAME_TIME;
            e->frame = (e->frame + 1) % ENEMY_FRAMES;
        }

        // --- Dano por contato: distância < soma dos raios. ---
        // Os i-frames do player (#18) evitam dano contínuo excessivo.
        if (Vector2LengthSqr(Vector2Subtract(player->position, e->position)) < contactDist2)
        {
            PlayerTakeDamage(player, ENEMY_DAMAGE);
        }
    }
}

void EnemyDraw(void)
{
    float destW = ENEMY_FRAME_W * ENEMY_SCALE;
    float destH = ENEMY_FRAME_H * ENEMY_SCALE;
    Vector2 origin = { destW / 2.0f, destH / 2.0f }; // centraliza o sprite

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        const Enemy *e = &enemies[i];
        if (!e->active) continue;

        Rectangle source = {
            (float)(e->frame * ENEMY_FRAME_W),
            0.0f,
            (float)ENEMY_FRAME_W,
            (float)ENEMY_FRAME_H,
        };
        Rectangle dest = { e->position.x, e->position.y, destW, destH };
        DrawTexturePro(enemyTex, source, dest, origin, 0.0f, WHITE);
    }
}

bool EnemyClosest(Vector2 from, Vector2 *outPos)
{
    bool  found = false;
    float bestDist2 = 0.0f;
    Vector2 best = { 0.0f, 0.0f };

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        const Enemy *e = &enemies[i];
        if (!e->active) continue;

        // Distância ao quadrado (sem sqrt): suficiente pra ordenar por proximidade.
        float d2 = Vector2DistanceSqr(from, e->position);
        if (!found || d2 < bestDist2)
        {
            found = true;
            bestDist2 = d2;
            best = e->position;
        }
    }

    if (found && outPos) *outPos = best;
    return found;
}

bool EnemyHitAt(Vector2 point, float radius, Vector2 *outKilledPos)
{
    const float r2 = radius * radius;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        // Acerto quando o centro do inimigo está dentro do raio (dist² < raio²).
        if (Vector2DistanceSqr(point, e->position) < r2)
        {
            if (outKilledPos) *outKilledPos = e->position; // onde dropar a gema
            e->active = false; // mata: libera o slot do pool pra reciclagem
            return true;       // consome só o primeiro inimigo atingido
        }
    }
    return false;
}

void EnemyUnload(void)
{
    UnloadTexture(enemyTex); // libera a textura antes de fechar a janela
}
