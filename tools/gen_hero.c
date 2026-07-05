// gen_hero.c — Gerador procedural do spritesheet do herói.
//
// Monta uma GRADE de 2 linhas x 8 colunas de frames 32x32 (spritesheet final
// 256x64) com um humanoide simples em pixel art e exporta para assets/hero.png.
//
// LAYOUT DO SPRITESHEET (2 linhas de 32x32):
//   Linha 0 (y = 0)  -> IDLE:  4 frames de "respiração" (colunas 0..3)
//                       O corpo/cabeça sobe e desce 1px num ciclo suave
//                       (0 = neutro, 1 = sobe, 2 = neutro, 3 = desce).
//                       Colunas 4..7 ficam VAZIAS (o idle só usa 4 frames).
//   Linha 1 (y = 32) -> WALK:  8 frames de caminhada (colunas 0..7)
//                       Pernas e braços alternam em OPOSIÇÃO (braço esquerdo
//                       à frente quando a perna direita avança) e o corpo
//                       sobe/desce 1px a cada passo.
//
// No main.c, o `source` seleciona x = frame*32 e y = estado*32
// (estado 0 = idle, estado 1 = walk).
//
// Usa apenas a Image API da raylib, que é CPU-side: NÃO abre janela nem exige
// contexto OpenGL. Assim roda de forma confiável dentro do toolchain UCRT64.
//
// Compilar e rodar UMA vez (no shell UCRT64):
//   gcc tools/gen_hero.c -o gen_hero.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//   ./gen_hero.exe

#include "raylib.h"

#define FRAME_W    32
#define FRAME_H    32
#define COLS       8               // colunas por linha (walk usa todas as 8)
#define ROWS       2               // linha 0 = idle, linha 1 = walk
#define IDLE_FRAMES 4
#define WALK_FRAMES 8
#define SHEET_W    (FRAME_W * COLS)  // 256
#define SHEET_H    (FRAME_H * ROWS)  // 64

// Paleta do herói (pixel art placeholder, mas reconhecível como humanoide).
static const Color SKIN  = (Color){ 240, 190, 140, 255 };  // pele
static const Color HAIR  = (Color){  80,  50,  30, 255 };  // cabelo marrom
static const Color SHIRT = (Color){  60, 120, 220, 255 };  // camisa azul
static const Color PANTS = (Color){  50,  50,  70, 255 };  // calça escura
static const Color BOOTS = (Color){  30,  30,  30, 255 };  // botas

// Desenha o humanoide dentro do frame cujo canto sup-esq. é (ox, oy).
//
// Parâmetros de pose (todos relativos ao frame 32x32):
//   bob      -> deslocamento vertical do corpo inteiro (respiração/passo).
//   legLift  -> +1 = perna DIREITA à frente, -1 = perna ESQUERDA à frente,
//               0 = pernas juntas (idle).
//   armSwing -> balanço dos braços em OPOSIÇÃO às pernas:
//               +1 = braço ESQUERDO à frente / direito atrás,
//               -1 = braço DIREITO à frente / esquerdo atrás,
//               0 = braços neutros (idle).
static void DrawHero(Image *img, int ox, int oy, int bob, int legLift, int armSwing)
{
    // Aplica o bob (respiração/passo) a tudo que é "corpo" (cabeça..cintura).
    // As pernas ficam ancoradas no chão; só o tronco/cabeça sobe e desce.
    int y = oy + bob;

    // --- Cabeça ---
    // Cabelo (topo).
    ImageDrawRectangle(img, ox + 11, y + 3,  10, 3, HAIR);
    // Rosto.
    ImageDrawRectangle(img, ox + 11, y + 6,  10, 7, SKIN);
    // Cabelo nas laterais.
    ImageDrawRectangle(img, ox + 11, y + 6,  2, 4, HAIR);
    ImageDrawRectangle(img, ox + 19, y + 6,  2, 4, HAIR);
    // Olhos.
    ImageDrawRectangle(img, ox + 13, y + 9,  2, 2, BLACK);
    ImageDrawRectangle(img, ox + 17, y + 9,  2, 2, BLACK);

    // --- Tronco / camisa ---
    ImageDrawRectangle(img, ox + 10, y + 13, 12, 8, SHIRT);

    // --- Braços (tom de pele), um de cada lado do tronco ---
    // O braço "à frente" desce mais baixo (mais avançado no passo); o de
    // "trás" fica mais recuado/curto. No idle (armSwing == 0) ficam simétricos.
    // Braço ESQUERDO (lado esquerdo da tela, x menor).
    int lArmY = y + 13 + (armSwing > 0 ? 1 : 0);
    int lArmH = 7 + (armSwing > 0 ? 1 : (armSwing < 0 ? -1 : 0));
    ImageDrawRectangle(img, ox + 7,  lArmY, 3, lArmH, SKIN);
    // Braço DIREITO (lado direito da tela, x maior).
    int rArmY = y + 13 + (armSwing < 0 ? 1 : 0);
    int rArmH = 7 + (armSwing < 0 ? 1 : (armSwing > 0 ? -1 : 0));
    ImageDrawRectangle(img, ox + 22, rArmY, 3, rArmH, SKIN);

    // --- Cintura / calça (base do tronco) ---
    ImageDrawRectangle(img, ox + 10, y + 21, 12, 2, PANTS);

    // --- Pernas ---
    // Ancoradas ao chão do frame (não sofrem o bob completo, só metade dele,
    // pra dar a sensação de peso). A que está "à frente" fica mais reta e
    // longa; a de "trás" recua e encurta.
    int legTop = oy + 23 + (bob > 0 ? 1 : 0);  // pernas seguem só metade do bob

    if (legLift == 0)
    {
        // Idle: pernas juntas e paradas.
        ImageDrawRectangle(img, ox + 11, legTop, 4, 7, PANTS);
        ImageDrawRectangle(img, ox + 17, legTop, 4, 7, PANTS);
        ImageDrawRectangle(img, ox + 11, legTop + 7, 4, 2, BOOTS);
        ImageDrawRectangle(img, ox + 17, legTop + 7, 4, 2, BOOTS);
    }
    else if (legLift > 0)
    {
        // Perna DIREITA à frente (avança para a direita, longa/reta).
        ImageDrawRectangle(img, ox + 18, legTop, 4, 7, PANTS);
        ImageDrawRectangle(img, ox + 18, legTop + 7, 4, 2, BOOTS);
        // Perna ESQUERDA atrás (recuada, mais curta).
        ImageDrawRectangle(img, ox + 11, legTop + 1, 4, 5, PANTS);
        ImageDrawRectangle(img, ox + 11, legTop + 6, 4, 2, BOOTS);
    }
    else // legLift < 0
    {
        // Perna ESQUERDA à frente (avança para a esquerda, longa/reta).
        ImageDrawRectangle(img, ox + 10, legTop, 4, 7, PANTS);
        ImageDrawRectangle(img, ox + 10, legTop + 7, 4, 2, BOOTS);
        // Perna DIREITA atrás (recuada, mais curta).
        ImageDrawRectangle(img, ox + 18, legTop + 1, 4, 5, PANTS);
        ImageDrawRectangle(img, ox + 18, legTop + 6, 4, 2, BOOTS);
    }
}

int main(void)
{
    // Fundo TRANSPARENTE.
    Image sheet = GenImageColor(SHEET_W, SHEET_H, BLANK);

    // ---------------------------------------------------------------
    // LINHA 0 — IDLE (respiração): 4 frames, corpo sobe/desce 1px.
    // bob: 0 (neutro), -1 (sobe/inspira), 0 (neutro), +1 (desce/expira).
    // Pernas juntas, braços neutros o tempo todo.
    // ---------------------------------------------------------------
    int idleBob[IDLE_FRAMES] = { 0, -1, 0, +1 };
    for (int f = 0; f < IDLE_FRAMES; f++)
    {
        DrawHero(&sheet, f * FRAME_W, 0 * FRAME_H, idleBob[f], 0, 0);
    }

    // ---------------------------------------------------------------
    // LINHA 1 — WALK (caminhada): 8 frames.
    // Ciclo de passada com pernas e braços em oposição, e bob do corpo.
    //   legLift:  perna à frente ao longo do ciclo.
    //   armSwing: SEMPRE oposto a legLift (braço esq. à frente quando
    //             perna direita avança) -> armSwing = -legLift.
    //   bob:      corpo sobe (-1) na passada, desce (0/+1) na aterrissagem.
    // ---------------------------------------------------------------
    // Frame:          0    1    2    3    4    5    6    7
    int legLift[WALK_FRAMES] = {  0,  +1,  +1,   0,   0,  -1,  -1,   0 };
    int walkBob[WALK_FRAMES] = {  0,  -1,   0,  +1,   0,  -1,   0,  +1 };
    for (int f = 0; f < WALK_FRAMES; f++)
    {
        DrawHero(&sheet, f * FRAME_W, 1 * FRAME_H, walkBob[f], legLift[f], -legLift[f]);
    }

    bool ok = ExportImage(sheet, "assets/hero.png");
    UnloadImage(sheet);

    if (!ok)
    {
        TraceLog(LOG_ERROR, "Falha ao exportar assets/hero.png");
        return 1;
    }

    TraceLog(LOG_INFO,
        "Spritesheet exportado: assets/hero.png (256x64: linha0=idle 4f, linha1=walk 8f, frames 32x32)");
    return 0;
}
