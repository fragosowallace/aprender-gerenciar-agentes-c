// levelup.h — sistema de nível + menu de upgrade (issue #24).
//
// Fluxo: o XP acumulado (XpGetTotal) enche a barra do nível atual. Ao atingir
// o limiar, o jogo sobe de nível e ABRE o menu de upgrade — momento em que o
// main.c pausa o gameplay (estado LEVELUP). O menu mostra 3 opções sorteadas;
// o jogador escolhe com as teclas 1/2/3, o efeito é aplicado e o jogo retoma.
//
// O limiar CRESCE a cada nível: threshold(n) = XP_BASE + n * XP_INCREMENT
// (custo do nível n -> n+1). Ver levelup.c.
#ifndef LEVELUP_H
#define LEVELUP_H

#include "raylib.h"
#include "player.h"

// Inicializa o sistema: nível 1, barra vazia, menu fechado.
// Chame UMA vez após InitWindow() e antes do game loop.
void LevelUpInit(void);

// Verifica se o XP acumulado atingiu o limiar do nível atual. Se sim, sobe de
// nível, ABRE o menu (sorteando 3 upgrades) e retorna true — sinal pro main.c
// entrar no estado LEVELUP. Caso contrário retorna false.
// Chame no estado PLAYING, DEPOIS de XpUpdate.
bool LevelUpPoll(void);

// Atualiza o menu de upgrade (só faz sentido enquanto ativo): lê as teclas
// 1/2/3, aplica o upgrade escolhido no player/arma/xp e FECHA o menu.
// Chame no estado LEVELUP. Use LevelUpMenuActive() pra saber quando sair.
void LevelUpMenuUpdate(Player *player);

// Retorna true enquanto o menu de upgrade estiver aberto (aguardando escolha).
bool LevelUpMenuActive(void);

// Desenha o HUD do nível: sempre a barra de progresso (topo da tela) e, quando
// o menu está ativo, o overlay semitransparente com as 3 opções. Deve ser
// chamado em espaço de tela (FORA de BeginMode2D/EndMode2D).
void LevelUpDraw(void);

// Retorna o nível atual (começa em 1).
int LevelUpGetLevel(void);

#endif // LEVELUP_H
