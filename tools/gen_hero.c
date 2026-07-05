// gen_hero.c — Gerador procedural do spritesheet do herói.
//
// Monta um strip HORIZONTAL de 4 frames de 16x16 (spritesheet final 64x16) com
// um humanoide simples em pixel art e exporta para assets/hero.png.
//
// Usa apenas a Image API da raylib, que é CPU-side: NÃO abre janela nem exige
// contexto OpenGL. Assim roda de forma confiável dentro do toolchain UCRT64.
//
// Frames:
//   0: idle (pernas juntas)
//   1: passo (perna esquerda à frente)
//   2: idle (pernas juntas)  -> volta ao neutro no meio do ciclo
//   3: passo (perna direita à frente)
//
// Compilar e rodar UMA vez (no shell UCRT64):
//   gcc tools/gen_hero.c -o gen_hero.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//   ./gen_hero.exe

#include "raylib.h"

#define FRAME_W 16
#define FRAME_H 16
#define FRAMES  4
#define SHEET_W (FRAME_W * FRAMES)  // 64
#define SHEET_H FRAME_H             // 16

// Paleta do herói (pixel art placeholder, mas reconhecível como humanoide).
static const Color SKIN  = (Color){ 240, 190, 140, 255 };  // pele
static const Color HAIR  = (Color){  80,  50,  30, 255 };  // cabelo marrom
static const Color SHIRT = (Color){  60, 120, 220, 255 };  // camisa azul
static const Color PANTS = (Color){  50,  50,  70, 255 };  // calça escura
static const Color BOOTS = (Color){  30,  30,  30, 255 };  // botas

// Desenha um humanoide dentro do frame de índice `frame`, com origem no canto
// esquerdo em `ox`. `legPhase`: 0 = pernas juntas (idle/neutro), -1 = perna
// esquerda à frente, +1 = perna direita à frente.
static void DrawHero(Image *img, int ox, int legPhase)
{
    // Coordenadas relativas ao frame 16x16.
    // Cabelo (topo da cabeça).
    ImageDrawRectangle(img, ox + 5, 1, 6, 2, HAIR);
    // Cabeça (rosto).
    ImageDrawRectangle(img, ox + 5, 3, 6, 4, SKIN);
    // Cabelo nas laterais da cabeça.
    ImageDrawRectangle(img, ox + 5, 3, 1, 2, HAIR);
    ImageDrawRectangle(img, ox + 10, 3, 1, 2, HAIR);
    // Olhos.
    ImageDrawPixel(img, ox + 6, 5, BLACK);
    ImageDrawPixel(img, ox + 9, 5, BLACK);

    // Tronco / camisa.
    ImageDrawRectangle(img, ox + 4, 7, 8, 4, SHIRT);
    // Braços (nas laterais do tronco), tom de pele.
    ImageDrawRectangle(img, ox + 3, 7, 1, 4, SKIN);
    ImageDrawRectangle(img, ox + 12, 7, 1, 4, SKIN);

    // Cintura / calça (base do tronco).
    ImageDrawRectangle(img, ox + 4, 11, 8, 1, PANTS);

    // Pernas: dependem da fase do passo.
    // Neutro: duas pernas centralizadas.
    // Passo: uma perna avança (desloca 1px horizontal) e encurta 1px.
    if (legPhase == 0)
    {
        // Perna esquerda.
        ImageDrawRectangle(img, ox + 5, 12, 2, 3, PANTS);
        // Perna direita.
        ImageDrawRectangle(img, ox + 9, 12, 2, 3, PANTS);
        // Botas.
        ImageDrawRectangle(img, ox + 5, 15, 2, 1, BOOTS);
        ImageDrawRectangle(img, ox + 9, 15, 2, 1, BOOTS);
    }
    else if (legPhase < 0)
    {
        // Perna esquerda avança (frente): mais à esquerda.
        ImageDrawRectangle(img, ox + 4, 12, 2, 3, PANTS);
        ImageDrawRectangle(img, ox + 4, 15, 2, 1, BOOTS);
        // Perna direita atrás: mais ao centro/direita e recuada.
        ImageDrawRectangle(img, ox + 9, 12, 2, 2, PANTS);
        ImageDrawRectangle(img, ox + 9, 14, 2, 1, BOOTS);
    }
    else // legPhase > 0
    {
        // Perna direita avança (frente): mais à direita.
        ImageDrawRectangle(img, ox + 10, 12, 2, 3, PANTS);
        ImageDrawRectangle(img, ox + 10, 15, 2, 1, BOOTS);
        // Perna esquerda atrás.
        ImageDrawRectangle(img, ox + 5, 12, 2, 2, PANTS);
        ImageDrawRectangle(img, ox + 5, 14, 2, 1, BOOTS);
    }
}

int main(void)
{
    // Fundo TRANSPARENTE.
    Image sheet = GenImageColor(SHEET_W, SHEET_H, BLANK);

    // Fases das pernas por frame: idle, passo-esq, neutro, passo-dir.
    int phases[FRAMES] = { 0, -1, 0, +1 };

    for (int f = 0; f < FRAMES; f++)
    {
        DrawHero(&sheet, f * FRAME_W, phases[f]);
    }

    bool ok = ExportImage(sheet, "assets/hero.png");
    UnloadImage(sheet);

    if (!ok)
    {
        TraceLog(LOG_ERROR, "Falha ao exportar assets/hero.png");
        return 1;
    }

    TraceLog(LOG_INFO, "Spritesheet exportado: assets/hero.png (64x16, 4 frames)");
    return 0;
}
