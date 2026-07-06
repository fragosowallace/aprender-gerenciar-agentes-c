// hud.h — HUD do jogo (issue #25): barra de vida, timer de sobrevivencia e kills.
//
// Desenhado em ESPACO DE TELA (fora de BeginMode2D). NAO desenha XP/nivel: isso
// e responsabilidade do levelup.c (barra de XP no topo-esquerda, ~y50-86). Para
// evitar sobreposicao, a barra de vida fica LOGO ABAIXO da barra de XP.
#ifndef HUD_H
#define HUD_H

#include "player.h"

// Inicializa o HUD: zera o tempo de sobrevivencia acumulado.
// Chame UMA vez apos InitWindow() e antes do game loop.
void HudInit(void);

// Acumula o tempo de sobrevivencia (dt em segundos). Chame SOMENTE no estado
// PLAYING, para que o tempo parado no menu de level up nao conte.
void HudUpdate(float dt);

// Desenha o HUD em espaco de tela (chame FORA de BeginMode2D/EndMode2D):
//  - barra de vida (topo-esquerda, abaixo da barra de XP), verde->vermelho;
//  - timer de sobrevivencia mm:ss (topo-centro);
//  - contador de kills (topo-direita) via EnemyGetKillCount().
void HudDraw(const Player *player);

// Retorna o tempo de sobrevivencia acumulado (em segundos). Usado pela tela de
// Game Over pra mostrar quanto tempo o jogador durou.
float HudGetTime(void);

#endif // HUD_H
