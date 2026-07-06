// hud.c — HUD do jogo: barra de vida, timer de sobrevivencia e contador de kills.
//
// Design: estado estatico encapsulado (mesmo padrao dos demais modulos). O unico
// estado guardado aqui e o tempo de sobrevivencia acumulado (survivalTime), que
// so avanca quando HudUpdate e chamado — e o main.c so o chama no estado PLAYING,
// entao o tempo parado no menu de level up nao conta.
//
// LAYOUT (espaco de tela, tudo fora de BeginMode2D). A barra de XP/nivel do
// levelup.c ocupa o topo-esquerda em ~x10..280, y50..86 ("Nivel X" em y50, barra
// em y70..86). Para nao colidir, posicionamos:
//   - Barra de vida : topo-esquerda LOGO ABAIXO da barra de XP -> x10, y94, 260x18
//   - Timer mm:ss   : topo-centro   -> y10
//   - Kills         : topo-direita  -> y10
// FPS/debug continuam no topo-esquerda em y10/y40, acima da barra de XP.
#include "hud.h"

#include "raylib.h"
#include "config.h"
#include "enemy.h" // EnemyGetKillCount()

// --- Layout da barra de vida ---
#define HP_BAR_X   10   // topo-esquerda, alinhada com a barra de XP
#define HP_BAR_Y   94   // abaixo da barra de XP (que vai ate y=86)
#define HP_BAR_W   260  // mesma largura da barra de XP, visualmente coeso
#define HP_BAR_H   18   // altura da barra

// Tempo de sobrevivencia acumulado, em segundos. So avanca no estado PLAYING.
static float survivalTime;

void HudInit(void)
{
    survivalTime = 0.0f;
}

void HudUpdate(float dt)
{
    // Chamado apenas no estado PLAYING: acumula o tempo de sobrevivencia.
    survivalTime += dt;
}

float HudGetTime(void)
{
    return survivalTime;
}

// Interpola verde->amarelo->vermelho conforme a fracao de vida (1.0 -> 0.0).
static Color HealthColor(float ratio)
{
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    // Verde (0,228,48) cheio -> Vermelho (230,41,55) vazio, passando por amarelo.
    unsigned char r = (unsigned char)((1.0f - ratio) * 230.0f + ratio * 0.0f);
    unsigned char g = (unsigned char)((1.0f - ratio) * 41.0f  + ratio * 228.0f);
    unsigned char b = (unsigned char)((1.0f - ratio) * 55.0f  + ratio * 48.0f);
    return (Color){ r, g, b, 255 };
}

void HudDraw(const Player *player)
{
    // --- Barra de vida (topo-esquerda, abaixo da barra de XP) ---
    float maxH = player->maxHealth;
    float curH = player->health;
    if (maxH < 1.0f)  maxH = 1.0f;   // evita divisao por zero
    if (curH < 0.0f)  curH = 0.0f;   // clamp inferior
    if (curH > maxH)  curH = maxH;   // clamp superior
    float ratio = curH / maxH;

    // Fundo escuro + preenchimento colorido proporcional + moldura.
    DrawRectangle(HP_BAR_X, HP_BAR_Y, HP_BAR_W, HP_BAR_H, (Color){ 40, 40, 40, 200 });
    DrawRectangle(HP_BAR_X, HP_BAR_Y, (int)(HP_BAR_W * ratio), HP_BAR_H, HealthColor(ratio));
    DrawRectangleLines(HP_BAR_X, HP_BAR_Y, HP_BAR_W, HP_BAR_H, RAYWHITE);

    // Numero da vida centralizado na barra (ex.: "70 / 100").
    const char *hpText = TextFormat("%d / %d", (int)(curH + 0.5f), (int)(maxH + 0.5f));
    int hpFont = 16;
    int hpTextW = MeasureText(hpText, hpFont);
    DrawText(hpText,
             HP_BAR_X + (HP_BAR_W - hpTextW) / 2,
             HP_BAR_Y + (HP_BAR_H - hpFont) / 2,
             hpFont, RAYWHITE);

    // --- Timer de sobrevivencia mm:ss (topo-centro) ---
    int totalSec = (int)survivalTime;
    int minutes  = totalSec / 60;
    int seconds  = totalSec % 60;
    const char *timeText = TextFormat("%02d:%02d", minutes, seconds);
    int timeFont = 28;
    int timeTextW = MeasureText(timeText, timeFont);
    DrawText(timeText, (SCREEN_WIDTH - timeTextW) / 2, 10, timeFont, RAYWHITE);

    // --- Contador de kills (topo-direita) ---
    const char *killText = TextFormat("Kills: %d", EnemyGetKillCount());
    int killFont = 22;
    int killTextW = MeasureText(killText, killFont);
    DrawText(killText, SCREEN_WIDTH - killTextW - 10, 12, killFont, RAYWHITE);
}
