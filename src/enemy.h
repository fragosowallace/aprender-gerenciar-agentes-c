// enemy.h — inimigos (slimes): pool fixo, spawn periódico, perseguição e dano.
#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "player.h"

// Inicializa o módulo: carrega assets/enemy.png com filtro POINT, zera o pool
// e o contador de spawn. Chame UMA vez após InitWindow() e antes do game loop.
void EnemyInit(void);

// Atualiza todos os inimigos ativos:
//  - spawn periódico de novos inimigos fora da tela ao redor do player;
//  - perseguição do player (movimento independente de framerate via dt);
//  - animação do slime;
//  - dano por contato (PlayerTakeDamage quando encosta no player).
void EnemyUpdate(float dt, Player *player);

// Desenha os inimigos ativos centralizados na posição, dentro da câmera.
// Deve ser chamado dentro de BeginMode2D/EndMode2D.
void EnemyDraw(void);

// Libera a textura dos inimigos. Chame antes de CloseWindow().
void EnemyUnload(void);

#endif // ENEMY_H
