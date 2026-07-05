// Vampire Survivors em C — init + game loop orquestrando os módulos.
// Config em config.h; herói em player.[ch]; cenário em world.[ch].

#include "raylib.h"

#include "config.h"
#include "player.h"
#include "world.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

    // Cria o player no centro do mundo (carrega o sprite).
    Player player;
    PlayerInit(&player);

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
        PlayerUpdate(&player, dt);

        // A câmera acompanha o player.
        camera.target = player.position;

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Mundo (afetado pela câmera): grid de referência + player.
        BeginMode2D(camera);
        WorldDraw();
        PlayerDraw(&player);
        EndMode2D();

        // HUD (espaço de tela, fora da câmera).
        DrawText("M3: WASD/setas pra mover o herói", 10, 40, 20, RAYWHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    PlayerUnload(&player);
    CloseWindow();
    return 0;
}
