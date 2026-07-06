// weapon.h — arma automática: auto-fire de projéteis mirando o inimigo mais
// próximo (estilo Vampire Survivors). O player não mira manualmente.
#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"
#include "player.h"

// Inicializa o módulo: zera o pool de projéteis e o timer de disparo.
// Chame UMA vez após InitWindow() e antes do game loop.
void WeaponInit(void);

// Atualiza a arma:
//  - conta o intervalo de disparo; ao completar, se houver inimigo ativo,
//    dispara um projétil do player em direção ao inimigo mais próximo (som de tiro);
//  - move os projéteis ativos (independente de FPS via dt) e expira por tempo de vida;
//  - colisão projétil↔inimigo: mata o inimigo (som de acerto) e consome o projétil.
void WeaponUpdate(float dt, Player *player);

// Desenha os projéteis ativos (pequenos círculos claros). Deve ser chamado
// dentro de BeginMode2D/EndMode2D (afetado pela câmera).
void WeaponDraw(void);

// Reservado para simetria com os demais módulos. Sem recursos a liberar
// (pool estático, sem texturas próprias). Chame antes de CloseWindow().
void WeaponUnload(void);

// Upgrade de cadência: multiplica o intervalo entre disparos por `factor`
// (use < 1.0 pra atirar MAIS rápido). O intervalo é limitado a um piso mínimo
// pra não zerar. Usado pelo menu de level up (issue #24).
void WeaponUpgradeFireRate(float factor);

#endif // WEAPON_H
