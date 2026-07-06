// Vampire Survivors em C — init + game loop orquestrando os módulos.
// Config em config.h; herói em player.[ch]; cenário em world.[ch].

#include "raylib.h"

#include "config.h"
#include "player.h"
#include "world.h"
#include "enemy.h"
#include "weapon.h"
#include "xp.h"
#include "levelup.h"
#include "hud.h"
#include "audio.h"

// Estados do jogo: durante LEVELUP o gameplay congela e o menu de upgrade
// aguarda a escolha do jogador (1/2/3). Voltamos a PLAYING ao escolher.
typedef enum GameState {
    STATE_PLAYING = 0,
    STATE_LEVELUP,
} GameState;

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

    // Gemas de XP: pool de gemas dropadas pelos inimigos mortos.
    XpInit();

    // Level up: barra de XP por nível + menu de upgrade (pausa o jogo).
    LevelUpInit();

    // HUD: barra de vida, timer de sobrevivência e contador de kills.
    HudInit();

    // Estado atual da máquina de estados do jogo.
    GameState state = STATE_PLAYING;

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

        if (state == STATE_PLAYING)
        {
            // Gameplay normal.
            PlayerUpdate(&player, dt);
            EnemyUpdate(dt, &player);   // spawn periódico + perseguição + dano por contato
            WeaponUpdate(dt, &player);  // auto-fire + movimento/colisão dos projéteis
            XpUpdate(dt, &player);      // ímã + coleta das gemas de XP
            HudUpdate(dt);              // acumula o tempo de sobrevivência (só no PLAYING)

            // A câmera acompanha o player.
            camera.target = player.position;

            // Subiu de nível? Abre o menu e pausa o gameplay.
            if (LevelUpPoll())
                state = STATE_LEVELUP;
        }
        else // STATE_LEVELUP
        {
            // Gameplay CONGELADO: só o menu roda. Lê 1/2/3, aplica e fecha.
            LevelUpMenuUpdate(&player);
            if (!LevelUpMenuActive())
                state = STATE_PLAYING; // escolheu: retoma o jogo
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);

        // Mundo (afetado pela câmera): grid de referência + player.
        BeginMode2D(camera);
        WorldDraw();
        XpDraw();             // gemas no chão, abaixo dos inimigos e do player
        EnemyDraw();          // inimigos antes do player, pra o herói ficar por cima
        WeaponDraw();         // projéteis entre inimigos e player (dentro da câmera)
        PlayerDraw(&player);
        EndMode2D();

        // HUD (espaço de tela, fora da câmera).
        DrawFPS(10, 10);

        // HUD do jogo: vida, timer, kills. Antes do LevelUpDraw pra o overlay
        // do menu de level up ficar por cima (não sobrepõe o HUD ao menu).
        HudDraw(&player);

        // Barra de nível (sempre) + overlay do menu quando em LEVELUP.
        // Desenhado por último, por cima do mundo que continua aparecendo.
        LevelUpDraw();

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
