// audio.h — Infra de áudio: inicializacao do device e efeitos sonoros (SFX).
//
// Camada fina sobre a API de audio da raylib. Isola InitAudioDevice/LoadSound/
// PlaySound/CloseAudioDevice para que o resto do jogo dependa apenas destas
// funcoes. Os eventos (tiro, dano, etc.) serao conectados em ondas futuras;
// aqui esta so a base.
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

// Inicializa o device de audio (InitAudioDevice). Chame uma vez no startup,
// APOS InitWindow. Enquanto o device nao estiver pronto, LoadSound/PlaySound
// nao produzem som.
void AudioInit(void);

// Carrega um efeito sonoro de um arquivo .wav/.ogg (LoadSound). Retorna um
// Sound; em caso de falha a raylib devolve um Sound "vazio" (stream nulo) que
// simplesmente nao toca. Descarregue com UnloadSound quando nao precisar mais.
Sound AudioLoad(const char *path);

// Toca o efeito sonoro uma vez (PlaySound). Chamadas repetidas reiniciam o som.
void AudioPlay(Sound s);

// Finaliza o device de audio (CloseAudioDevice). Chame uma vez no shutdown,
// depois de descarregar os Sounds.
void AudioShutdown(void);

#endif // AUDIO_H
