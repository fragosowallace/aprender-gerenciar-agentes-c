// world.h — cenário: chão em pixel art tileado + borda do mundo.
#ifndef WORLD_H
#define WORLD_H

// Carrega a textura do chão (assets/ground.png) com filtro POINT.
// Chame UMA vez após InitWindow() e antes do game loop.
void WorldInit(void);

// Desenha o chão texturizado (tile repetido cobrindo a área jogável) e a
// borda do mundo. Deve ser chamado dentro de BeginMode2D/EndMode2D (afetado
// pela câmera).
void WorldDraw(void);

// Libera a textura do chão. Chame antes de CloseWindow().
void WorldUnload(void);

#endif // WORLD_H
