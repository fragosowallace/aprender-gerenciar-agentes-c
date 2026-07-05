// audio.c — Implementacao da camada de audio (ver audio.h).
//
// Wrappers finos sobre a API de audio da raylib. Sem estado global proprio: o
// device de audio e gerenciado internamente pela raylib. Estas funcoes podem
// ainda nao ser chamadas por ninguem (os eventos entram em ondas futuras), mas
// ja compilam e linkam junto do jogo pelo wildcard do Makefile.
#include "audio.h"

void AudioInit(void)
{
    InitAudioDevice();
}

Sound AudioLoad(const char *path)
{
    return LoadSound(path);
}

void AudioPlay(Sound s)
{
    PlaySound(s);
}

void AudioShutdown(void)
{
    CloseAudioDevice();
}
