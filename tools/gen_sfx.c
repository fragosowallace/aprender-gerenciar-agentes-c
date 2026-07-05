// gen_sfx.c — Gerador procedural de efeitos sonoros (SFX) placeholder.
//
// Preenche manualmente buffers de amostras PCM 16-bit e os exporta como .wav
// via a API Wave/ExportWave da raylib. Tudo CPU-side: NAO abre janela nem exige
// device de audio nem contexto OpenGL. Roda de forma confiavel no toolchain
// UCRT64.
//
// Efeitos gerados (curtos, ~0.12s, 22050 Hz, 16-bit mono):
//   assets/sfx_shoot.wav -> onda QUADRADA com pitch descendente + decay (tiro).
//   assets/sfx_hit.wav   -> onda SENOIDE grave com decay rapido (impacto/dano).
//
// Compilar e rodar UMA vez (no shell UCRT64):
//   gcc tools/gen_sfx.c -o gen_sfx.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//   ./gen_sfx.exe

#include "raylib.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define SAMPLE_RATE   22050
#define SAMPLE_SIZE   16       // bits por amostra (PCM 16-bit)
#define CHANNELS      1        // mono

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Monta um Wave PCM 16-bit mono a partir de um buffer de amostras int16 ja
// preenchido. O Wave assume a posse do buffer: UnloadWave o libera depois.
static Wave MakeWave(int16_t *samples, unsigned int frameCount)
{
    Wave w = { 0 };
    w.frameCount = frameCount;
    w.sampleRate = SAMPLE_RATE;
    w.sampleSize = SAMPLE_SIZE;
    w.channels   = CHANNELS;
    w.data       = samples;
    return w;
}

// Exporta um Wave placeholder gerado por 'fill'. 'fill' recebe o buffer e a
// contagem de frames e o preenche com amostras int16. Retorna true no sucesso.
static bool GenAndExport(const char *path, float seconds,
                         void (*fill)(int16_t *, unsigned int))
{
    unsigned int frameCount = (unsigned int)(seconds * SAMPLE_RATE);
    int16_t *samples = (int16_t *)calloc(frameCount, sizeof(int16_t));
    if (samples == NULL)
    {
        TraceLog(LOG_ERROR, "gen_sfx: falha ao alocar buffer para %s", path);
        return false;
    }

    fill(samples, frameCount);

    Wave w = MakeWave(samples, frameCount);
    bool ok = ExportWave(w, path);
    UnloadWave(w);  // libera 'samples'

    if (!ok) TraceLog(LOG_ERROR, "gen_sfx: falha ao exportar %s", path);
    return ok;
}

// --- SHOOT: onda quadrada, pitch descendente + decay exponencial. ---
static void FillShoot(int16_t *s, unsigned int n)
{
    const double f0 = 880.0;   // Hz inicial
    const double f1 = 220.0;   // Hz final
    double phase = 0.0;
    for (unsigned int i = 0; i < n; i++)
    {
        double t    = (double)i / n;                 // 0..1 ao longo do efeito
        double freq = f0 + (f1 - f0) * t;            // glissando descendente
        phase += 2.0 * M_PI * freq / SAMPLE_RATE;
        double square = (sin(phase) >= 0.0) ? 1.0 : -1.0;
        double env    = exp(-5.0 * t);               // decay exponencial
        double v      = square * env * 0.35;         // headroom p/ evitar clip
        s[i] = (int16_t)(v * 32767.0);
    }
}

// --- HIT: senoide grave com decay rapido (thump de impacto). ---
static void FillHit(int16_t *s, unsigned int n)
{
    const double freq = 140.0;  // Hz — grave
    for (unsigned int i = 0; i < n; i++)
    {
        double t   = (double)i / n;
        double v   = sin(2.0 * M_PI * freq * i / SAMPLE_RATE);
        double env = exp(-9.0 * t);                  // decay mais rapido
        s[i] = (int16_t)(v * env * 0.6 * 32767.0);
    }
}

int main(void)
{
    int failures = 0;

    if (!GenAndExport("assets/sfx_shoot.wav", 0.12f, FillShoot)) failures++;
    if (!GenAndExport("assets/sfx_hit.wav",   0.15f, FillHit))   failures++;

    if (failures > 0)
    {
        TraceLog(LOG_ERROR, "gen_sfx: %d efeito(s) falharam", failures);
        return 1;
    }

    TraceLog(LOG_INFO, "gen_sfx: exportados assets/sfx_shoot.wav e assets/sfx_hit.wav "
                       "(22050 Hz, 16-bit mono)");
    return 0;
}
