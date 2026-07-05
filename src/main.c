// Vampire Survivors em C — M1: janela + game loop
// Issue #1

#include "raylib.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

    // Game loop: roda até o usuário fechar a janela ou apertar ESC.
    while (!WindowShouldClose())
    {
        // --- Update ---
        // (ainda não há estado de jogo; entra no M2)

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        const char *msg = "M1: janela + loop OK";
        int fontSize = 20;
        int textWidth = MeasureText(msg, fontSize);
        DrawText(msg, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2 - fontSize / 2,
                 fontSize, RAYWHITE);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
