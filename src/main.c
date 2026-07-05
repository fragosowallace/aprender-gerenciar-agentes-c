// Vampire Survivors em C — M2: movimento do player + câmera 2D
// Issue #2

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Tamanho do "mundo" — bem maior que a tela, pra a câmera ter pra onde seguir.
#define WORLD_WIDTH  2000
#define WORLD_HEIGHT 2000
#define GRID_SPACING 64  // distância entre as linhas do grid de referência

// Struct do player: posição no mundo, velocidade (px/s) e tamanho (lado do quadrado).
typedef struct Player {
    Vector2 position;  // posição no mundo (centro do retângulo)
    float   speed;     // velocidade em pixels por segundo
    float   size;      // lado do retângulo (px)
} Player;

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

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

        // Normaliza a direção pra a diagonal não andar mais rápido.
        if (Vector2Length(dir) > 0.0f)
        {
            dir = Vector2Normalize(dir);
            // position += dir * speed * dt
            player.position = Vector2Add(player.position,
                                         Vector2Scale(dir, player.speed * dt));
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

        // Player: retângulo de destaque, desenhado a partir do canto (position é o centro).
        DrawRectangle((int)(player.position.x - player.size / 2.0f),
                      (int)(player.position.y - player.size / 2.0f),
                      (int)player.size, (int)player.size, GOLD);

        EndMode2D();

        // HUD (espaço de tela, fora da câmera).
        DrawText("M2: WASD/setas pra mover", 10, 40, 20, RAYWHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
