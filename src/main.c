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

// Estados do jogo:
//  - PLAYING : gameplay normal.
//  - LEVELUP : o gameplay congela e o menu de upgrade aguarda a escolha do
//              jogador (1/2/3). Voltamos a PLAYING ao escolher.
//  - GAMEOVER: o player morreu; o mundo congela (segue desenhado por baixo) e
//              um overlay mostra o resultado. ENTER reinicia a partida.
typedef enum GameState {
    STATE_PLAYING = 0,
    STATE_LEVELUP,
    STATE_GAMEOVER,
} GameState;

// Reinicia a partida ao estado inicial SEM recarregar assets: chama os *Reset
// dos módulos que carregam textura/som (player, enemy, weapon) e reutiliza os
// *Init dos módulos sem asset (xp, levelup, hud), que já resetam o estado.
// Usado tanto na inicialização quanto no restart pós Game Over, garantindo um
// único ponto de verdade pro "estado inicial de partida".
static void RestartGame(Player *player)
{
    PlayerReset(player); // posição no centro, vida cheia, i-frames limpos
    EnemyReset();        // pool + spawnTimer + killCount zerados
    WeaponReset();       // projéteis + fireTimer + cadência inicial
    XpInit();            // gemas + total de XP zerados (sem asset)
    LevelUpInit();       // nível 1, barra vazia, menu fechado (sem asset)
    HudInit();           // timer de sobrevivência zerado (sem asset)
}

// Desenha o overlay de Game Over em espaço de tela (FORA de BeginMode2D): o
// mundo segue desenhado por baixo, com um escurecimento e o resultado por cima.
static void DrawGameOver(void)
{
    // Escurece a tela inteira por cima do mundo congelado.
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 190 });

    // Título "GAME OVER".
    const char *title = "GAME OVER";
    int titleFont = 60;
    int titleW = MeasureText(title, titleFont);
    DrawText(title, (SCREEN_WIDTH - titleW) / 2, SCREEN_HEIGHT / 2 - 130,
             titleFont, (Color){ 230, 41, 55, 255 });

    // Tempo sobrevivido (mm:ss), reaproveitando o timer do HUD.
    int totalSec = (int)HudGetTime();
    const char *timeText = TextFormat("Tempo sobrevivido: %02d:%02d",
                                      totalSec / 60, totalSec % 60);
    int statFont = 26;
    int timeW = MeasureText(timeText, statFont);
    DrawText(timeText, (SCREEN_WIDTH - timeW) / 2, SCREEN_HEIGHT / 2 - 30,
             statFont, RAYWHITE);

    // Kills, reaproveitando o getter existente do módulo de inimigos.
    const char *killText = TextFormat("Kills: %d", EnemyGetKillCount());
    int killW = MeasureText(killText, statFont);
    DrawText(killText, (SCREEN_WIDTH - killW) / 2, SCREEN_HEIGHT / 2 + 6,
             statFont, RAYWHITE);

    // Instrução de reinício.
    const char *hint = "Pressione ENTER para reiniciar";
    int hintFont = 22;
    int hintW = MeasureText(hint, hintFont);
    DrawText(hint, (SCREEN_WIDTH - hintW) / 2, SCREEN_HEIGHT / 2 + 70,
             hintFont, (Color){ 200, 200, 120, 255 });
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vampire Survivors - C");
    SetTargetFPS(60);

    // Áudio: inicia o device ANTES de qualquer AudioLoad (a arma carrega os
    // SFX de tiro/acerto na sua init).
    AudioInit();

    // --- Carga ÚNICA de assets (texturas/sons) ---
    // Estes *Init carregam recursos e NÃO devem rodar de novo no restart (senão
    // recarregam/vazam o asset). O reset de ESTADO deles fica nos *Reset abaixo.

    // Cria o player no centro do mundo (carrega o sprite).
    Player player;
    PlayerInit(&player);

    // Cenário: carrega a textura do chão (tile de grama/terra).
    WorldInit();

    // Inimigos: carrega o sprite do slime e prepara o pool/spawn.
    EnemyInit();

    // Arma automática: pool de projéteis + auto-fire (carrega os SFX).
    WeaponInit();

    // --- Estado inicial de partida (também usado no restart pós Game Over) ---
    // Zera player/inimigos/arma (estado) e (re)inicia xp/levelup/hud, que não
    // têm asset. Chamado aqui e no restart pelo MESMO helper.
    RestartGame(&player);

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

            // Morreu? Congela o gameplay e vai pra tela de Game Over.
            // (Checado antes do level up: se morreu, não abre menu de upgrade.)
            if (PlayerIsDead(&player))
            {
                state = STATE_GAMEOVER;
            }
            // Subiu de nível? Abre o menu e pausa o gameplay.
            else if (LevelUpPoll())
            {
                state = STATE_LEVELUP;
            }
        }
        else if (state == STATE_LEVELUP)
        {
            // Gameplay CONGELADO: só o menu roda. Lê 1/2/3, aplica e fecha.
            LevelUpMenuUpdate(&player);
            if (!LevelUpMenuActive())
                state = STATE_PLAYING; // escolheu: retoma o jogo
        }
        else // STATE_GAMEOVER
        {
            // Gameplay CONGELADO: nada é atualizado (mundo segue desenhado por
            // baixo). ENTER reinicia a partida ao estado inicial e volta a jogar.
            // Só sai daqui com ENTER — sem loop de re-entrada.
            if (IsKeyPressed(KEY_ENTER))
            {
                RestartGame(&player);
                camera.target = player.position; // recentra na posição inicial
                state = STATE_PLAYING;
            }
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

        // Overlay de Game Over por cima de tudo (mundo congelado ao fundo).
        if (state == STATE_GAMEOVER)
            DrawGameOver();

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
