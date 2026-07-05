// world.c — desenho do cenário de referência (grid + borda).
#include "world.h"

#include "raylib.h"
#include "config.h"

void WorldDraw(void)
{
    // Grid: linhas verticais e horizontais a cada GRID_SPACING px.
    Color gridColor = (Color){ 40, 40, 40, 255 };  // cinza escuro discreto
    for (int x = 0; x <= WORLD_WIDTH; x += GRID_SPACING)
        DrawLine(x, 0, x, WORLD_HEIGHT, gridColor);
    for (int y = 0; y <= WORLD_HEIGHT; y += GRID_SPACING)
        DrawLine(0, y, WORLD_WIDTH, y, gridColor);

    // Borda do mundo pra deixar clara a área jogável.
    DrawRectangleLines(0, 0, WORLD_WIDTH, WORLD_HEIGHT, DARKGRAY);
}
