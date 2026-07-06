// xp.h — gemas de XP: drop na morte do inimigo, ímã (atração ao player) e
// coleta. Pool fixo de gemas, mesmo padrão dos demais módulos (estado estático,
// sem alocação no loop, ciclo Init/Update/Draw). O total acumulado é consultado
// via XpGetTotal() (será usado pelo level up / HUD nas issues #24/#25).
#ifndef XP_H
#define XP_H

#include "raylib.h"
#include "player.h"

// Inicializa o módulo: zera o pool de gemas e o total de XP.
// Chame UMA vez após InitWindow() e antes do game loop.
void XpInit(void);

// Dropa uma gema na posição `pos` (tipicamente onde um inimigo morreu).
// Reaproveita um slot livre do pool; se estiver cheio, ignora silenciosamente.
void XpSpawn(Vector2 pos);

// Atualiza as gemas ativas:
//  - ímã: gema perto do player (< raio de ímã) é atraída na direção dele;
//  - coleta: gema encostada no player (< raio de coleta) é coletada,
//    incrementando o total de XP e desativando a gema.
void XpUpdate(float dt, Player *player);

// Desenha as gemas ativas (losango ciano em pixel art). Deve ser chamado
// dentro de BeginMode2D/EndMode2D (afetado pela câmera), no chão — abaixo
// dos inimigos e do player.
void XpDraw(void);

// Retorna o total de XP acumulado (soma das gemas já coletadas).
int XpGetTotal(void);

// Upgrade de ímã: multiplica o raio de atração das gemas por `factor`
// (use > 1.0 pra atrair de mais longe). Usado pelo menu de level up (issue #24).
void XpUpgradeMagnet(float factor);

#endif // XP_H
