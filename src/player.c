// player.c — implementação do herói: carga, input, movimento e desenho.
#include "player.h"

#include "raymath.h"
#include "config.h"

// --- Sprite do herói ---
// Spritesheet em grade de 2 linhas x 8 colunas, frames de 32x32 (256x64):
//   linha 0 (y=0)  = IDLE, 4 frames de respiração;
//   linha 1 (y=32) = WALK, 8 frames de caminhada.
// O `source` seleciona x = frame*32, y = estado*32.
#define HERO_FRAME_W    32    // largura de cada frame no spritesheet (px)
#define HERO_FRAME_H    32    // altura de cada frame (px)
#define HERO_SCALE      2     // cada pixel do sprite = 2px na tela (escala inteira)

// Estados de animação (também são o índice da LINHA no spritesheet).
#define ANIM_IDLE       0
#define ANIM_WALK       1

// Contagem de frames e tempo por frame de cada estado.
#define IDLE_FRAMES     4
#define WALK_FRAMES     8
#define IDLE_FRAME_TIME 0.18f // respiração lenta e suave
#define WALK_FRAME_TIME 0.10f // passada mais ágil

// --- Vida e dano ---
#define PLAYER_MAX_HEALTH  100.0f // vida cheia no início
#define PLAYER_INVULN_TIME 0.5f   // duração dos i-frames após tomar dano (s)
#define PLAYER_BLINK_RATE  12.0f  // trocas de alpha por segundo enquanto invulnerável

void PlayerInit(Player *p)
{
    // Player começa no centro do mundo.
    p->position   = (Vector2){ WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f };
    p->speed      = 250.0f;
    p->size       = 24.0f;

    // Estado inicial da animação do herói.
    p->state      = ANIM_IDLE;  // estado atual (idle/walk) = linha no sheet
    p->frame      = 0;          // frame atual dentro do estado
    p->timer      = 0.0f;       // acumulador de tempo pra troca de frame
    p->facingLeft = false;      // direção do flip horizontal

    // Vida cheia e sem invulnerabilidade no início.
    p->maxHealth   = PLAYER_MAX_HEALTH;
    p->health      = PLAYER_MAX_HEALTH;
    p->invulnTimer = 0.0f;

    // Spritesheet do herói: grade 2x8 de frames 32x32 (linha0=idle, linha1=walk).
    p->texture = LoadTexture("assets/hero.png");
    // Filtro POINT: mantém o pixel art nítido, sem borrar ao escalar.
    SetTextureFilter(p->texture, TEXTURE_FILTER_POINT);
}

void PlayerUpdate(Player *p, float dt)
{
    // Monta o vetor de direção a partir do input (WASD + setas).
    Vector2 dir = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    dir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  dir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  dir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) dir.x += 1.0f;

    bool moving = (Vector2Length(dir) > 0.0f);

    // Normaliza a direção pra a diagonal não andar mais rápido.
    if (moving)
    {
        dir = Vector2Normalize(dir);
        // position += dir * speed * dt
        p->position = Vector2Add(p->position,
                                 Vector2Scale(dir, p->speed * dt));

        // Flip horizontal: só atualiza quando há movimento horizontal claro,
        // pra não "resetar" a direção ao andar só na vertical.
        if (dir.x < -0.01f) p->facingLeft = true;
        else if (dir.x > 0.01f) p->facingLeft = false;
    }

    // Animação com estados separados (idle vs walk), cada um com sua
    // contagem de frames e timing próprios.
    // - Idle NÃO congela: anima continuamente a respiração.
    // - Walk percorre o ciclo de passada só enquanto há movimento.
    int newState = moving ? ANIM_WALK : ANIM_IDLE;
    if (newState != p->state)
    {
        // Troca de estado: reinicia o ciclo pra começar limpo.
        p->state = newState;
        p->frame = 0;
        p->timer = 0.0f;
    }

    int   frameCount = (p->state == ANIM_WALK) ? WALK_FRAMES : IDLE_FRAMES;
    float frameTime  = (p->state == ANIM_WALK) ? WALK_FRAME_TIME : IDLE_FRAME_TIME;

    p->timer += dt;
    if (p->timer >= frameTime)
    {
        p->timer -= frameTime;
        p->frame = (p->frame + 1) % frameCount;
    }

    // Decrementa o timer de i-frames (invulnerabilidade), sem passar de zero.
    if (p->invulnTimer > 0.0f)
    {
        p->invulnTimer -= dt;
        if (p->invulnTimer < 0.0f) p->invulnTimer = 0.0f;
    }
}

void PlayerTakeDamage(Player *p, float amount)
{
    // Enquanto invulnerável (i-frames ativos), ignora o dano.
    if (p->invulnTimer > 0.0f) return;

    p->health -= amount;
    if (p->health < 0.0f) p->health = 0.0f;  // clamp >= 0

    // Ativa a janela de invulnerabilidade.
    p->invulnTimer = PLAYER_INVULN_TIME;
}

bool PlayerIsDead(const Player *p)
{
    return p->health <= 0.0f;
}

void PlayerDraw(const Player *p)
{
    // Herói: sprite animado em escala inteira, centralizado na posição.
    // source seleciona a coluna (frame) e a linha (estado);
    // largura negativa espelha (flip horizontal).
    Rectangle source = {
        (float)(p->frame * HERO_FRAME_W),
        (float)(p->state * HERO_FRAME_H),
        p->facingLeft ? -(float)HERO_FRAME_W : (float)HERO_FRAME_W,
        (float)HERO_FRAME_H,
    };
    float destW = HERO_FRAME_W * HERO_SCALE;
    float destH = HERO_FRAME_H * HERO_SCALE;
    Rectangle dest = { p->position.x, p->position.y, destW, destH };
    Vector2 origin = { destW / 2.0f, destH / 2.0f };  // centraliza o sprite

    // Feedback visual de invulnerabilidade: enquanto os i-frames estão ativos,
    // pisca alternando o alpha e tinge levemente de vermelho. Fora disso é WHITE
    // normal, então o desenho padrão não muda.
    Color tint = WHITE;
    if (p->invulnTimer > 0.0f)
    {
        // Alterna visível/apagado em intervalos regulares (efeito de piscar).
        float phase = p->invulnTimer * PLAYER_BLINK_RATE;
        bool  bright = ((int)phase % 2) == 0;
        tint = bright ? (Color){ 255, 120, 120, 255 }   // tom avermelhado, visível
                      : (Color){ 255, 120, 120, 90 };   // semi-transparente
    }
    DrawTexturePro(p->texture, source, dest, origin, 0.0f, tint);
}

void PlayerUnload(Player *p)
{
    UnloadTexture(p->texture);  // libera a textura antes de fechar a janela
}
