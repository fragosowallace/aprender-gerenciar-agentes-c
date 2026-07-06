// weapon.c — arma automática: pool fixo de projéteis + auto-fire mirando o
// inimigo ativo mais próximo. Segue o padrão dos outros módulos: estado
// estático encapsulado, pool fixo (sem alocação no loop), ciclo de vida
// Init/Update/Draw/Unload.
#include "weapon.h"

#include "raymath.h"
#include "enemy.h"
#include "audio.h"
#include "xp.h"

// --- Pool ---
#define MAX_PROJECTILES   200    // capacidade máxima do pool de projéteis

// --- Parâmetros da arma ---
#define FIRE_INTERVAL     0.5f   // intervalo inicial entre disparos (s)
#define FIRE_INTERVAL_MIN 0.08f  // piso do intervalo (não atira mais rápido que isso)
#define PROJECTILE_SPEED  400.0f // velocidade do projétil (px/s)
#define PROJECTILE_LIFE   1.5f   // tempo de vida do projétil (s) antes de sumir
#define PROJECTILE_RADIUS 5.0f   // raio de colisão/desenho do projétil (px)

// Um projétil do pool: posição e velocidade no mundo, flag de ativo e o tempo
// de vida restante (conta pra baixo; ao chegar a 0 o projétil some).
typedef struct Projectile {
    Vector2 position; // posição no mundo (centro)
    Vector2 velocity; // px/s (direção * PROJECTILE_SPEED)
    bool    active;   // slot ocupado?
    float   life;     // tempo de vida restante (s)
} Projectile;

// Estado do módulo (estático, encapsulado — mesmo padrão de enemy.c/world.c).
static Projectile projectiles[MAX_PROJECTILES];
static float      fireTimer;    // acumula dt; ao passar de fireInterval, dispara
static float      fireInterval; // intervalo atual entre disparos (s); reduzido por upgrades
static Sound      shootSfx;     // som do disparo
static Sound      hitSfx;       // som do acerto/morte do inimigo

// Ativa um slot livre do pool com posição e velocidade dadas. Se o pool
// estiver cheio, ignora silenciosamente (sem overflow).
static void SpawnProjectile(Vector2 position, Vector2 velocity)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        if (projectiles[i].active) continue;

        projectiles[i].position = position;
        projectiles[i].velocity = velocity;
        projectiles[i].active   = true;
        projectiles[i].life     = PROJECTILE_LIFE;
        return;
    }
    // Pool cheio: nenhum slot livre. Ignora este disparo.
}

void WeaponReset(void)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
        projectiles[i].active = false;

    fireTimer    = 0.0f;
    fireInterval = FIRE_INTERVAL; // restaura a cadência inicial (descarta upgrades)
    // NÃO toca em shootSfx/hitSfx: reset é só de estado, sem reload de asset.
}

void WeaponInit(void)
{
    // Estado inicial (pool, fireTimer, fireInterval) — compartilhado com o restart.
    WeaponReset();

    // Sons carregados UMA vez aqui (o device já foi iniciado por AudioInit no
    // main); o restart usa WeaponReset e não recarrega os SFX.
    shootSfx = AudioLoad("assets/sfx_shoot.wav");
    hitSfx   = AudioLoad("assets/sfx_hit.wav");
}

void WeaponUpdate(float dt, Player *player)
{
    // --- Auto-fire: dispara em direção ao inimigo ativo mais próximo. ---
    fireTimer += dt;
    while (fireTimer >= fireInterval)
    {
        fireTimer -= fireInterval;

        Vector2 targetPos;
        if (EnemyClosest(player->position, &targetPos))
        {
            Vector2 toTarget = Vector2Subtract(targetPos, player->position);
            if (Vector2LengthSqr(toTarget) > 0.0001f)
            {
                Vector2 dir = Vector2Normalize(toTarget);
                Vector2 vel = Vector2Scale(dir, PROJECTILE_SPEED);
                SpawnProjectile(player->position, vel);
                AudioPlay(shootSfx);
            }
        }
        // Sem inimigos: não dispara (economiza projéteis).
    }

    // --- Move projéteis, expira por tempo de vida e checa colisão. ---
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        Projectile *p = &projectiles[i];
        if (!p->active) continue;

        // Move independente de FPS.
        p->position = Vector2Add(p->position, Vector2Scale(p->velocity, dt));

        // Tempo de vida: ao esgotar, o projétil some.
        p->life -= dt;
        if (p->life <= 0.0f)
        {
            p->active = false;
            continue;
        }

        // Colisão projétil↔inimigo: se acertar, mata o inimigo (som), dropa
        // uma gema de XP na posição dele e consome o projétil.
        Vector2 killedPos;
        if (EnemyHitAt(p->position, PROJECTILE_RADIUS, &killedPos))
        {
            p->active = false;
            AudioPlay(hitSfx);
            XpSpawn(killedPos);
        }
    }
}

void WeaponDraw(void)
{
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        const Projectile *p = &projectiles[i];
        if (!p->active) continue;

        // Projétil como pequeno círculo claro, dentro da câmera.
        DrawCircleV(p->position, PROJECTILE_RADIUS, RAYWHITE);
    }
}

void WeaponUnload(void)
{
    // Descarrega os sons carregados na init (antes de AudioShutdown no main).
    UnloadSound(shootSfx);
    UnloadSound(hitSfx);
}

void WeaponUpgradeFireRate(float factor)
{
    // Reduz o intervalo entre disparos (factor < 1 => atira mais rápido),
    // respeitando um piso pra não zerar a cadência.
    fireInterval *= factor;
    if (fireInterval < FIRE_INTERVAL_MIN) fireInterval = FIRE_INTERVAL_MIN;
}
