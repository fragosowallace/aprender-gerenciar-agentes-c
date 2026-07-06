// levelup.c — sistema de nível + menu de upgrade.
//
// Design (mesmo padrão dos demais módulos: estado estático encapsulado, ciclo
// Init/Poll/Update/Draw):
//
//  - Nível: começa em 1. O limiar do nível atual é
//        threshold(level) = XP_BASE + level * XP_INCREMENT
//    ou seja, o custo cresce a cada nível (nível 1 custa mais que a base,
//    nível 2 mais ainda, etc.). Guardamos `xpConsumed`, o XP "gasto" nos
//    níveis anteriores; o progresso no nível atual é XpGetTotal() - xpConsumed.
//    Ao progresso alcançar o limiar, sobe de nível e desconta o limiar do
//    consumido (um único level up por chamada — o main pausa antes do próximo).
//
//  - Menu: ao subir de nível, sorteamos 3 upgrades DISTINTOS de um catálogo e
//    abrimos o menu. O jogador escolhe com 1/2/3; aplicamos o efeito e fechamos.
//    Só usamos upgrades compatíveis com a mecânica atual (sem +dano, já que
//    inimigos morrem em 1 acerto).
#include "levelup.h"

#include "weapon.h"
#include "xp.h"
#include "config.h"

// --- Fórmula de XP por nível ---
#define XP_BASE       5   // parcela fixa do limiar
#define XP_INCREMENT  5   // quanto o limiar cresce por nível

// --- Catálogo de upgrades ---
// Cada upgrade tem um id (pra aplicar o efeito) e um texto pro menu.
typedef enum UpgradeId {
    UP_FIRERATE = 0, // +cadência: arma atira mais rápido
    UP_SPEED,        // +velocidade de movimento do player
    UP_MAXHEALTH,    // +vida máxima (e cura)
    UP_MAGNET,       // +ímã: aumenta o raio de coleta de XP
    UPGRADE_COUNT,   // total de upgrades no catálogo (sentinela)
} UpgradeId;

// Parâmetros dos efeitos.
#define FIRERATE_FACTOR   0.85f  // intervalo *= 0.85 (atira ~18% mais rápido)
#define SPEED_BONUS       30.0f  // +30 px/s de velocidade
#define MAXHEALTH_BONUS   20.0f  // +20 de vida máxima (também cura 20)
#define MAGNET_FACTOR     1.30f  // raio de ímã *= 1.30

// Textos exibidos no menu (indexados por UpgradeId).
static const char *UPGRADE_LABELS[UPGRADE_COUNT] = {
    "+Cadencia de tiro",
    "+Velocidade de movimento",
    "+Vida maxima (e cura)",
    "+Ima (raio de coleta de XP)",
};

// --- Estado do módulo ---
#define MENU_OPTIONS  3  // quantas opções o menu oferece

static int  level;               // nível atual (começa em 1)
static int  xpConsumed;          // XP "gasto" nos níveis já concluídos
static bool menuActive;          // menu de upgrade aberto?
static int  options[MENU_OPTIONS]; // ids dos upgrades sorteados pro menu atual

// Limiar (custo) do nível atual pra subir pro próximo.
static int LevelThreshold(void)
{
    return XP_BASE + level * XP_INCREMENT;
}

// Sorteia MENU_OPTIONS upgrades DISTINTOS do catálogo para o menu.
static void RollOptions(void)
{
    bool used[UPGRADE_COUNT] = { false };
    for (int i = 0; i < MENU_OPTIONS; i++)
    {
        int id;
        do {
            id = GetRandomValue(0, UPGRADE_COUNT - 1);
        } while (used[id]);
        used[id]   = true;
        options[i] = id;
    }
}

// Aplica o efeito de um upgrade pelo id.
static void ApplyUpgrade(int id, Player *player)
{
    switch (id)
    {
        case UP_FIRERATE:
            WeaponUpgradeFireRate(FIRERATE_FACTOR);
            break;
        case UP_SPEED:
            player->speed += SPEED_BONUS;
            break;
        case UP_MAXHEALTH:
            player->maxHealth += MAXHEALTH_BONUS;
            player->health    += MAXHEALTH_BONUS; // cura junto
            if (player->health > player->maxHealth)
                player->health = player->maxHealth;
            break;
        case UP_MAGNET:
            XpUpgradeMagnet(MAGNET_FACTOR);
            break;
        default:
            break; // id inválido: no-op
    }
}

void LevelUpInit(void)
{
    level      = 1;
    xpConsumed = 0;
    menuActive = false;
    for (int i = 0; i < MENU_OPTIONS; i++)
        options[i] = 0;
}

bool LevelUpPoll(void)
{
    // Enquanto o menu está aberto o gameplay está pausado; não sobe de novo.
    if (menuActive) return false;

    int progress = XpGetTotal() - xpConsumed; // XP dentro do nível atual
    if (progress >= LevelThreshold())
    {
        xpConsumed += LevelThreshold(); // desconta o custo deste nível
        level++;                        // sobe de nível
        RollOptions();                  // sorteia as 3 opções do menu
        menuActive = true;              // abre o menu (main entra em LEVELUP)
        return true;
    }
    return false;
}

void LevelUpMenuUpdate(Player *player)
{
    if (!menuActive) return;

    // Teclas 1/2/3 escolhem a opção correspondente.
    int choice = -1;
    if (IsKeyPressed(KEY_ONE)   || IsKeyPressed(KEY_KP_1)) choice = 0;
    if (IsKeyPressed(KEY_TWO)   || IsKeyPressed(KEY_KP_2)) choice = 1;
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) choice = 2;

    if (choice >= 0)
    {
        ApplyUpgrade(options[choice], player);
        menuActive = false; // fecha o menu -> main volta pra PLAYING
    }
}

bool LevelUpMenuActive(void)
{
    return menuActive;
}

void LevelUpDraw(void)
{
    // --- Barra de progresso do nível (sempre visível, topo da tela) ---
    int progress  = XpGetTotal() - xpConsumed;
    int threshold = LevelThreshold();
    if (progress < 0) progress = 0;
    if (progress > threshold) progress = threshold;

    const int barX = 10, barY = 70, barW = 260, barH = 16;
    DrawRectangle(barX, barY, barW, barH, (Color){ 40, 40, 40, 200 });
    float ratio = (threshold > 0) ? (float)progress / (float)threshold : 0.0f;
    DrawRectangle(barX, barY, (int)(barW * ratio), barH, SKYBLUE);
    DrawRectangleLines(barX, barY, barW, barH, RAYWHITE);
    DrawText(TextFormat("Nivel %d", level), barX, barY - 20, 20, RAYWHITE);
    DrawText(TextFormat("%d / %d XP", progress, threshold),
             barX + barW + 10, barY - 2, 18, RAYWHITE);

    // --- Overlay do menu (só quando ativo) ---
    if (!menuActive) return;

    // Escurece a tela inteira (semitransparente) por cima do mundo.
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 170 });

    // Painel central com as 3 opções.
    const int panelW = 520, panelH = 260;
    const int panelX = (SCREEN_WIDTH  - panelW) / 2;
    const int panelY = (SCREEN_HEIGHT - panelH) / 2;
    DrawRectangle(panelX, panelY, panelW, panelH, (Color){ 20, 20, 30, 235 });
    DrawRectangleLines(panelX, panelY, panelW, panelH, SKYBLUE);

    const char *title = TextFormat("NIVEL %d - escolha um upgrade", level);
    int titleW = MeasureText(title, 26);
    DrawText(title, panelX + (panelW - titleW) / 2, panelY + 20, 26, RAYWHITE);

    for (int i = 0; i < MENU_OPTIONS; i++)
    {
        int oy = panelY + 70 + i * 56;
        const char *line = TextFormat("[%d]  %s", i + 1, UPGRADE_LABELS[options[i]]);
        DrawText(line, panelX + 30, oy, 24, (Color){ 220, 220, 120, 255 });
    }

    const char *hint = "Pressione 1, 2 ou 3";
    int hintW = MeasureText(hint, 18);
    DrawText(hint, panelX + (panelW - hintW) / 2, panelY + panelH - 34, 18,
             (Color){ 180, 180, 180, 255 });
}

int LevelUpGetLevel(void)
{
    return level;
}
