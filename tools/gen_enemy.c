// gen_enemy.c — Gerador procedural do spritesheet do inimigo (SLIME roxo).
//
// Monta uma GRADE de 1 linha x 4 colunas de frames 16x16 (spritesheet final
// 64x16) com um "slime"/blob simples em pixel art e exporta para
// assets/enemy.png.
//
// LAYOUT DO SPRITESHEET (1 linha de 16x16):
//   Linha 0 (y = 0) -> WALK: 4 frames de "andar saltitante" (colunas 0..3).
//                      O slime pulsa: agacha e estica num ciclo de gelatina
//                      (0 = neutro, 1 = esticado/alto, 2 = neutro,
//                       3 = agachado/achatado). Dá a leitura de um blob
//                      pulando enquanto se move.
//
// No main.c, o `source` seleciona x = frame*16 e y = 0 (só um estado: walk).
//
// PALETA: tons de ROXO (corpo) + destaque avermelhado (olhos), distinta do
// herói (que é azul/pele) para leitura rápida de "isto é um inimigo".
//
// Usa apenas a Image API da raylib, que é CPU-side: NÃO abre janela nem exige
// contexto OpenGL. Assim roda de forma confiável dentro do toolchain UCRT64.
//
// Compilar e rodar UMA vez (no shell UCRT64):
//   gcc tools/gen_enemy.c -o gen_enemy.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//   ./gen_enemy.exe

#include "raylib.h"

#define FRAME_W     16
#define FRAME_H     16
#define COLS        4                  // 4 frames de walk
#define ROWS        1                  // apenas 1 estado: walk
#define WALK_FRAMES 4
#define SHEET_W     (FRAME_W * COLS)   // 64
#define SHEET_H     (FRAME_H * ROWS)   // 16

// Paleta do slime (roxo), distinta do herói.
static const Color BODY_DARK  = (Color){  80,  30, 120, 255 };  // roxo escuro (sombra/base)
static const Color BODY_MAIN  = (Color){ 140,  60, 200, 255 };  // roxo principal
static const Color BODY_LIGHT = (Color){ 180, 120, 235, 255 };  // roxo claro (brilho)
static const Color EYE_WHITE  = (Color){ 240, 240, 250, 255 };  // branco do olho
static const Color EYE_RED    = (Color){ 220,  40,  60, 255 };  // pupila avermelhada (malvado)

// Desenha o slime dentro do frame cujo canto sup-esq. é (ox, oy).
//
// Parâmetro de pose (relativo ao frame 16x16):
//   squash -> deformação de gelatina:
//              0  = corpo neutro (altura média).
//             +1 = ESTICADO: mais alto e estreito (topo do pulo).
//             -1 = AGACHADO: mais baixo e largo (achatado no chão).
static void DrawSlime(Image *img, int ox, int oy, int squash)
{
    // Altura do corpo varia com o squash; a base fica sempre ancorada no chão
    // (parte de baixo do frame), então só o topo sobe/desce.
    // Largura varia em oposição à altura (conservação de "gelatina").
    int bodyH = 9 + squash;            // 8 (agachado) .. 10 (esticado)
    int inset = (squash < 0) ? 0 : 1;  // agachado = mais largo (menos inset)

    int left   = ox + 2 + inset;       // borda esquerda do corpo
    int right  = ox + 14 - inset;      // borda direita (exclusiva)
    int width  = right - left;
    int baseY  = oy + 15;              // linha do chão (última do frame)
    int topY   = baseY - bodyH;        // topo do corpo

    // --- Corpo (bloco arredondado por "cortes" nos cantos) ---
    // Bloco principal.
    ImageDrawRectangle(img, left, topY, width, bodyH, BODY_MAIN);

    // Base mais escura (contato com o chão / sombra própria).
    ImageDrawRectangle(img, left, baseY - 2, width, 2, BODY_DARK);

    // Arredonda os cantos superiores removendo 1px de cada lado (transparente).
    ImageDrawRectangle(img, left,        topY, 1, 1, BLANK);
    ImageDrawRectangle(img, right - 1,   topY, 1, 1, BLANK);
    // Arredonda os cantos inferiores.
    ImageDrawRectangle(img, left,        baseY - 1, 1, 1, BLANK);
    ImageDrawRectangle(img, right - 1,   baseY - 1, 1, 1, BLANK);

    // Brilho (highlight) no alto à esquerda, dá volume de gelatina.
    ImageDrawRectangle(img, left + 1, topY + 1, 2, 2, BODY_LIGHT);

    // --- Olhos ---
    // Dois olhos brancos com pupila vermelha, na metade superior do corpo.
    int eyeY = topY + 3;
    int eyeLx = left + 2;
    int eyeRx = right - 4;
    ImageDrawRectangle(img, eyeLx, eyeY, 2, 2, EYE_WHITE);
    ImageDrawRectangle(img, eyeRx, eyeY, 2, 2, EYE_WHITE);
    // Pupilas (1px) — olhar levemente pra frente/baixo = expressão ameaçadora.
    ImageDrawPixel(img, eyeLx + 1, eyeY + 1, EYE_RED);
    ImageDrawPixel(img, eyeRx,     eyeY + 1, EYE_RED);
}

int main(void)
{
    // Fundo TRANSPARENTE.
    Image sheet = GenImageColor(SHEET_W, SHEET_H, BLANK);

    // ---------------------------------------------------------------
    // LINHA 0 — WALK (andar saltitante): 4 frames.
    // Ciclo de gelatina: neutro -> esticado -> neutro -> agachado.
    //   squash: 0 (neutro), +1 (esticado/pulo), 0 (neutro), -1 (agachado).
    // ---------------------------------------------------------------
    // Frame:              0   1   2   3
    int squash[WALK_FRAMES] = { 0, +1,  0, -1 };
    for (int f = 0; f < WALK_FRAMES; f++)
    {
        DrawSlime(&sheet, f * FRAME_W, 0 * FRAME_H, squash[f]);
    }

    bool ok = ExportImage(sheet, "assets/enemy.png");
    UnloadImage(sheet);

    if (!ok)
    {
        TraceLog(LOG_ERROR, "Falha ao exportar assets/enemy.png");
        return 1;
    }

    TraceLog(LOG_INFO,
        "Spritesheet exportado: assets/enemy.png (64x16: linha0=walk 4f, frames 16x16, slime roxo)");
    return 0;
}
