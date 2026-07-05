// Vampire Survivors em C — init + game loop orquestrando os módulos.
// Config em config.h; herói em player.[ch]; cenário em world.[ch].

#include "raylib.h"

#include "config.h"
#include "player.h"
#include "world.h"
#include "enemy.h"
#include "weapon.h"
#include "audio.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

    // Áudio: inicia o device ANTES de qualquer AudioLoad (a arma carrega os
    // SFX de tiro/acerto na sua init).
    AudioInit();

    // Cria o player no centro do mundo (carrega o sprite).
    Player player;
    PlayerInit(&player);

    // Cenário: carrega a textura do chão (tile de grama/terra).
    WorldInit();

    // Inimigos: carrega o sprite do slime e prepara o pool/spawn.
    EnemyInit();

    // Arma automática: pool de projéteis + auto-fire (carrega os SFX).
    WeaponInit();

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
        EnemyUpdate(dt, &player);   // spawn periódico + perseguição + dano por contato
        WeaponUpdate(dt, &player);  // auto-fire + movimento/colisão dos projéteis

        // A câmera acompanha o player.
        camera.target = player.position;

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Mundo (afetado pela câmera): grid de referência + player.
        BeginMode2D(camera);
        WorldDraw();
        EnemyDraw();          // inimigos antes do player, pra o herói ficar por cima
        WeaponDraw();         // projéteis entre inimigos e player (dentro da câmera)
        PlayerDraw(&player);
        EndMode2D();

        // HUD (espaço de tela, fora da câmera).
        DrawText("M3: WASD/setas pra mover o herói", 10, 40, 20, RAYWHITE);
        DrawFPS(10, 10);

        EndDrawing();
    }

    WeaponUnload();   // descarrega os SFX da arma antes de fechar o device
    PlayerUnload(&player);
    EnemyUnload();
    WorldUnload();
    AudioShutdown();  // fecha o device de áudio depois de descarregar os Sounds
    CloseWindow();
    return 0;
}
