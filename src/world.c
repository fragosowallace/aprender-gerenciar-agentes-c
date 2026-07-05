// world.c — desenho do cenário: chão em pixel art tileado + borda do mundo.
#include "world.h"

#include "raylib.h"
#include "config.h"

// Textura do tile de chão (assets/ground.png, 32x32 tileável), gerada por
// tools/gen_ground.c. Encapsulada aqui: carga em WorldInit, uso em WorldDraw,
// liberação em WorldUnload — mesmo padrão do player (PlayerInit/Unload).
static Texture2D groundTile;

void WorldInit(void)
{
    groundTile = LoadTexture("assets/ground.png");
    // Filtro POINT: mantém o pixel art nítido, sem borrar ao repetir/escalar.
    SetTextureFilter(groundTile, TEXTURE_FILTER_POINT);
}

void WorldDraw(void)
{
    // --- Chão: repete o tile cobrindo toda a área jogável (0..WORLD_*). ---
    // Percorre em passos do tamanho do tile e desenha uma cópia em cada célula.
    // O tile é tileável, então as cópias casam sem costura.
    int tw = groundTile.width  > 0 ? groundTile.width  : 32;
    int th = groundTile.height > 0 ? groundTile.height : 32;
    for (int y = 0; y < WORLD_HEIGHT; y += th)
        for (int x = 0; x < WORLD_WIDTH; x += tw)
            DrawTexture(groundTile, x, y, WHITE);

    // --- Linhas de grade sutis por cima, como referência de escala. ---
    Color gridColor = (Color){ 0, 0, 0, 40 };  // preto bem translúcido
    for (int x = 0; x <= WORLD_WIDTH; x += GRID_SPACING)
        DrawLine(x, 0, x, WORLD_HEIGHT, gridColor);
    for (int y = 0; y <= WORLD_HEIGHT; y += GRID_SPACING)
        DrawLine(0, y, WORLD_WIDTH, y, gridColor);

    // --- Borda do mundo pra deixar clara a área jogável. ---
    DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, DARKGRAY);
}

void WorldUnload(void)
{
    UnloadTexture(groundTile);
}
