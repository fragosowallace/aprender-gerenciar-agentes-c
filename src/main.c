// Vampire Survivors em C — M3: sprite do herói em pixel art com animação
// Issue #3

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Tamanho do "mundo" — bem maior que a tela, pra a câmera ter pra onde seguir.
#define WORLD_WIDTH  2000
#define WORLD_HEIGHT 2000
#define GRID_SPACING 64  // distância entre as linhas do grid de referência

// --- Sprite do herói ---
#define HERO_FRAME_W    16    // largura de cada frame no spritesheet (px)
#define HERO_FRAME_H    16    // altura de cada frame (px)
#define HERO_FRAMES     4     // nº de frames no strip horizontal
#define HERO_SCALE      3     // cada pixel do sprite = 3px na tela (escala inteira)
#define WALK_FRAME_TIME 0.12f // segundos por frame no ciclo de caminhada

// Struct do player: posição no mundo, velocidade (px/s) e tamanho (referência).
typedef struct Player {
    Vector2 position;  // posição no mundo (centro do sprite)
    float   speed;     // velocidade em pixels por segundo
    float   size;      // tamanho de referência (px), não mais desenhado
} Player;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

    // Spritesheet do herói: strip horizontal de 4 frames de 16x16.
    Texture2D heroTex = LoadTexture("assets/hero.png");
    // Filtro POINT: mantém o pixel art nítido, sem borrar ao escalar.
    SetTextureFilter(heroTex, TEXTURE_FILTER_POINT);

    // Estado da animação do herói.
    int   heroFrame  = 0;      // frame atual (0 = idle)
    float heroTimer  = 0.0f;   // acumulador de tempo pra troca de frame
    bool  facingLeft = false;  // direção do flip horizontal

    // Player começa no centro do mundo.
    Player player = {
        .position = { WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f },
        .speed    = 250.0f,
        .size     = 24.0f,
    };

    // Câmera 2D: segue o player, mantendo-o no centro da tela.
    Camera2D camera = {
        .target   = player.position,
        .offset   = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        .rotation = 0.0f,
        .zoom     = 1.0f,
    };

    // Game loop: roda até o usuário fechar a janela ou apertar ESC.
    while (!WindowShouldClose())
    {
        // --- Update ---
        float dt = GetFrameTime();  // tempo do frame: deixa o movimento independente de FPS

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
            player.position = Vector2Add(player.position,
                                         Vector2Scale(dir, player.speed * dt));

            // Flip horizontal: só atualiza quando há movimento horizontal claro,
            // pra não "resetar" a direção ao andar só na vertical.
            if (dir.x < -0.01f) facingLeft = true;
            else if (dir.x > 0.01f) facingLeft = false;
        }

        // Animação: parado => idle (frame 0); andando => percorre o ciclo.
        if (moving)
        {
            heroTimer += dt;
            if (heroTimer >= WALK_FRAME_TIME)
            {
                heroTimer -= WALK_FRAME_TIME;
                heroFrame = (heroFrame + 1) % HERO_FRAMES;
            }
        }
        else
        {
            heroFrame = 0;    // idle
            heroTimer = 0.0f; // reinicia o ciclo pro próximo passo começar limpo
        }

        // A câmera acompanha o player.
        camera.target = player.position;

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Mundo (afetado pela câmera): grid de referência + player.
        BeginMode2D(camera);

        // Grid: linhas verticais e horizontais a cada GRID_SPACING px.
        Color gridColor = (Color){ 40, 40, 40, 255 };  // cinza escuro discreto
        for (int x = 0; x <= WORLD_WIDTH; x += GRID_SPACING)
            DrawLine(x, 0, x, WORLD_HEIGHT, gridColor);
        for (int y = 0; y <= WORLD_HEIGHT; y += GRID_SPACING)
            DrawLine(0, y, WORLD_WIDTH, y, gridColor);

        // Borda do mundo pra deixar clara a área jogável.
        DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, DARKGRAY);

        // Herói: sprite animado em escala inteira, centralizado na posição.
        // source seleciona o frame; largura negativa espelha (flip horizontal).
        Rectangle source = {
            (float)(heroFrame * HERO_FRAME_W), 0.0f,
            facingLeft ? -(float)HERO_FRAME_W : (float)HERO_FRAME_W,
            (float)HERO_FRAME_H,
        };
        float destW = HERO_FRAME_W * HERO_SCALE;
        float destH = HERO_FRAME_H * HERO_SCALE;
        Rectangle dest = { player.position.x, player.position.y, destW, destH };
        Vector2 origin = { destW / 2.0f, destH / 2.0f };  // centraliza o sprite
        DrawTexturePro(heroTex, source, dest, origin, 0.0f, WHITE);

        EndMode2D();

        // HUD (espaço de tela, fora da câmera).
        DrawText("M3: WASD/setas pra mover o herói", 10, 40, 20, RAYWHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadTexture(heroTex);  // libera a textura antes de fechar a janela
    CloseWindow();
    return 0;
}
