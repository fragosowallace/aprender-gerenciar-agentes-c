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

// Consulta o inimigo ativo mais próximo de `from`. Se houver ao menos um
// inimigo ativo, escreve a posição dele em *outPos e retorna true; caso
// contrário retorna false (e não toca em *outPos). Usa distância ao quadrado
// (sem sqrt) para a comparação. Usado pela arma para mirar automaticamente.
bool EnemyClosest(Vector2 from, Vector2 *outPos);

// Mata o PRIMEIRO inimigo ativo cujo centro esteja dentro de `radius` de
// `point` (distância ao quadrado, sem sqrt). Retorna true se acertou/matou
// algum inimigo, false caso contrário. Usado pela colisão dos projéteis.
// Se acertou e `outKilledPos` não for NULL, escreve nele a posição do inimigo
// morto (usada pela arma para dropar uma gema de XP ali).
bool EnemyHitAt(Vector2 point, float radius, Vector2 *outKilledPos);

// Retorna quantos inimigos foram mortos desde EnemyInit(). Incrementa em 1 a
// cada morte dentro de EnemyHitAt(). Usado pelo HUD para o contador de kills.
int EnemyGetKillCount(void);

// Libera a textura dos inimigos. Chame antes de CloseWindow().
void EnemyUnload(void);

#endif // ENEMY_H
