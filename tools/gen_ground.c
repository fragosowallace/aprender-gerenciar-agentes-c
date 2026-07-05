// gen_ground.c — Gerador procedural do tile de chão (grama/terra).
//
// Gera UM tile 32x32 TILEÁVEL em pixel art (tons de verde com manchas de
// terra marrom e variação de pixels pra não ficar chapado) e exporta para
// assets/ground.png. O world.c carrega esse tile e o repete (tiled) por toda
// a área jogável do mundo.
//
// TILEABILIDADE: a variação usa um ruído baseado em (x, y) com wrap por 32,
// ou seja, o valor em x depende de x%32 — como o tile tem exatamente 32px de
// lado, a borda direita casa com a esquerda e a de baixo com a de cima. As
// manchas de terra também usam distância "toroidal" (com wrap em 32), então
// uma mancha perto de uma borda reaparece continuada na borda oposta. Assim,
// ao repetir o tile lado a lado, não aparecem costuras.
//
// Usa apenas a Image API da raylib, que é CPU-side: NÃO abre janela nem exige
// contexto OpenGL. Assim roda de forma confiável dentro do toolchain UCRT64.
//
// Compilar e rodar UMA vez (no shell UCRT64):
//   gcc tools/gen_ground.c -o gen_ground.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//   ./gen_ground.exe

#include "raylib.h"

#include <stdlib.h>  // abs

#define TILE_W 32
#define TILE_H 32

// Paleta do chão (grama em tons de verde, terra em tons de marrom).
static const Color GRASS_DARK  = (Color){  56, 104,  44, 255 };  // verde escuro (base)
static const Color GRASS_MID   = (Color){  74, 128,  56, 255 };  // verde médio
static const Color GRASS_LIGHT = (Color){  96, 152,  70, 255 };  // verde claro (destaque)
static const Color DIRT_DARK   = (Color){  92,  66,  40, 255 };  // terra escura
static const Color DIRT_LIGHT  = (Color){ 120,  88,  54, 255 };  // terra clara

// Hash determinístico e barato pra ruído por pixel. Recebe x,y JÁ reduzidos
// a [0,32) (wrap), então o resultado é periódico em 32 -> tileável.
static unsigned int Hash2(int x, int y)
{
    unsigned int h = (unsigned int)(x * 374761393 + y * 668265263);
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

int main(void)
{
    Image tile = GenImageColor(TILE_W, TILE_H, GRASS_DARK);

    // ------------------------------------------------------------------
    // 1) Textura da grama: pinta cada pixel num dos tons de verde a partir
    //    de um ruído periódico em 32 (usa x%32, y%32 -> casa nas bordas).
    // ------------------------------------------------------------------
    for (int y = 0; y < TILE_H; y++)
    {
        for (int x = 0; x < TILE_W; x++)
        {
            unsigned int n = Hash2(x % TILE_W, y % TILE_H) % 100u;
            Color c;
            if (n < 60)      c = GRASS_DARK;   // maioria: base escura
            else if (n < 88) c = GRASS_MID;    // médio
            else             c = GRASS_LIGHT;  // poucos pontos de destaque
            ImageDrawPixel(&tile, x, y, c);
        }
    }

    // ------------------------------------------------------------------
    // 2) Algumas manchas de terra, distribuídas de forma determinística.
    //    Centros escolhidos à mão; a distância usa wrap em 32 (toroidal),
    //    então manchas coladas na borda continuam do outro lado -> tileável.
    // ------------------------------------------------------------------
    struct { int cx, cy, r; } spots[] = {
        {  6,  8, 3 },
        { 22, 20, 4 },
        { 14, 27, 2 },
        { 30,  4, 2 },   // encostada na borda direita/superior -> testa o wrap
    };
    int nspots = (int)(sizeof(spots) / sizeof(spots[0]));

    for (int y = 0; y < TILE_H; y++)
    {
        for (int x = 0; x < TILE_W; x++)
        {
            for (int s = 0; s < nspots; s++)
            {
                // Distância com wrap toroidal em cada eixo (mín. entre direta
                // e "dando a volta" pelo tile de 32px).
                int dx = abs(x - spots[s].cx);
                if (dx > TILE_W / 2) dx = TILE_W - dx;
                int dy = abs(y - spots[s].cy);
                if (dy > TILE_H / 2) dy = TILE_H - dy;

                if (dx * dx + dy * dy <= spots[s].r * spots[s].r)
                {
                    // Terra com leve variação clara/escura (mesmo ruído).
                    unsigned int n = Hash2(x % TILE_W, y % TILE_H) % 100u;
                    ImageDrawPixel(&tile, x, y, (n < 70) ? DIRT_DARK : DIRT_LIGHT);
                    break;
                }
            }
        }
    }

    bool ok = ExportImage(tile, "assets/ground.png");
    UnloadImage(tile);

    if (!ok)
    {
        TraceLog(LOG_ERROR, "Falha ao exportar assets/ground.png");
        return 1;
    }

    TraceLog(LOG_INFO,
        "Tile de chao exportado: assets/ground.png (32x32, tileavel, grama+terra)");
    return 0;
}
