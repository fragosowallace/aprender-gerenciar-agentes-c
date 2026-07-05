# 🧛 Vampire Survivors em C — construído com Agentes + GitHub Issues

Um clone estilo *Vampire Survivors* em **pixel art**, escrito em **C** com a biblioteca
[**raylib**](https://www.raylib.com/), desenvolvido do zero como projeto de aprendizado.

> 🎯 **Objetivo duplo**: (1) aprender a orquestrar **agentes de IA** guiados por **GitHub Issues**,
> e (2) entender como um jogo 2D funciona por dentro, em C.

## 📋 Como este projeto é construído

Cada funcionalidade nasce de uma **Issue**, é implementada por um **agente** numa branch própria,
e entra no jogo via **Pull Request** revisado. Nada é feito "direto na main".
Veja o ciclo completo em [`docs/FLUXO-DE-TRABALHO.md`](docs/FLUXO-DE-TRABALHO.md).

O roteiro de marcos (M0 → M10) está em [`docs/PLANO.md`](docs/PLANO.md).

## 🛠️ Requisitos

- **MSYS2** com o toolchain UCRT64: `gcc` + `raylib` + `make`
  ```
  pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib make
  ```

## ▶️ Como compilar e rodar

Abra o terminal **MSYS2 UCRT64** (ou qualquer shell com `C:\msys64\ucrt64\bin` no PATH) na raiz do projeto:

```bash
make        # compila -> game.exe
make run    # compila e executa
make clean  # remove o executável
```

> **Nota (Windows/MSYS2):** fora do terminal MSYS2 UCRT64 (ex.: Git Bash), use
> **`mingw32-make`** no lugar de `make` — o `make` do pacote MSYS não repassa a
> variável `TMP` ao compilador e falha com "Cannot create temporary file". O
> `mingw32-make` (pacote `mingw-w64-ucrt-x86_64-make`) não tem esse problema.

## 📂 Estrutura

```
.
├── src/          # código-fonte C
├── assets/       # sprites, sons, fontes (pixel art)
├── docs/         # plano e fluxo de trabalho
├── Makefile      # build
└── README.md
```
