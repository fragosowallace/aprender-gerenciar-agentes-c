// player.h — o herói: struct de estado e ciclo de vida (init/update/draw/unload).
#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Struct do player: posição no mundo, velocidade (px/s), tamanho (referência),
// textura do spritesheet e estado da animação.
typedef struct Player {
    Vector2   position;   // posição no mundo (centro do sprite)
    float     speed;      // velocidade em pixels por segundo
    float     size;       // tamanho de referência (px), não mais desenhado
    Texture2D texture;    // spritesheet do herói (grade 2x8, frames 32x32)
    int       state;      // estado atual (idle/walk) = linha no sheet
    int       frame;      // frame atual dentro do estado
    float     timer;      // acumulador de tempo pra troca de frame
    bool      facingLeft; // direção do flip horizontal
} Player;

// Posiciona o player no centro do mundo e carrega o sprite.
void PlayerInit(Player *p);

// Input (WASD/setas), movimento normalizado por dt, animação e flip.
void PlayerUpdate(Player *p, float dt);

// Desenha o sprite (escala inteira, flip) na posição do mundo.
void PlayerDraw(const Player *p);

// Libera a textura.
void PlayerUnload(Player *p);

#endif // PLAYER_H
