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

Você pode compilar de **qualquer terminal** — MSYS2 UCRT64, Git Bash, **PowerShell**
ou **cmd** — desde que o **MSYS2 esteja instalado** (o `Makefile` usa o `sh` do
MSYS2 para rodar as receitas, então não importa de qual shell você o chama).

No **MSYS2 UCRT64** ou **Git Bash** (com `C:\msys64\ucrt64\bin` no PATH):

```bash
mingw32-make        # compila -> game.exe
mingw32-make run    # compila e executa
mingw32-make clean  # remove o executável
```

No **PowerShell** ou **cmd**, basta ter o `mingw32-make` no PATH. Exemplo de como
adicionar o toolchain ao PATH da sessão do PowerShell:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
mingw32-make        # compila -> game.exe
mingw32-make run    # compila e executa
mingw32-make clean  # remove o executável
```

> **Como isso funciona:** as receitas do `Makefile` usam comandos Unix (`rm`,
> `nm`, `grep`, `sed`, ...). Para que funcionem fora do MSYS2, o `Makefile`
> define `SHELL := C:/msys64/usr/bin/sh.exe` (e prefixa `C:/msys64/usr/bin` e
> `C:/msys64/ucrt64/bin` no `PATH`), forçando o make a executar as receitas com
> o `sh` do MSYS2 em vez do `cmd.exe`. Se você instalou o MSYS2 em outro lugar,
> passe a raiz na linha de comando: `mingw32-make MSYS2=D:/msys64`.

> **Nota (`make` vs `mingw32-make`):** use **`mingw32-make`** (pacote
> `mingw-w64-ucrt-x86_64-make`), e não o `make` do pacote MSYS — este não
> repassa a variável `TMP` ao compilador e falha com "Cannot create temporary
> file". O `mingw32-make` não tem esse problema, e é ele que funciona de
> qualquer terminal.

### 📦 Executável portável (autossuficiente)

O build é **estático**: a raylib, a GLFW e o runtime do gcc (`libgcc` /
`libwinpthread`) são **embutidos no próprio `game.exe`**. Ou seja, o executável
é **autossuficiente** e roda em qualquer Windows — inclusive com **duplo-clique
no Explorer**, fora do terminal MSYS2 — sem precisar de `libraylib.dll`,
`glfw3.dll`, `libgcc_s_seh-1.dll` nem `libwinpthread-1.dll` ao lado.

Você pode conferir que só restam DLLs de sistema do Windows:

```bash
objdump -p game.exe | grep -i "DLL Name"
# => apenas KERNEL32/USER32/GDI32/OPENGL32/WINMM/SHELL32 e api-ms-win-crt-*
```

> **Detalhe técnico:** o pacote `libraylib.a` do MSYS2 é compilado com
> `-DGLFW_DLL`, então chama a GLFW via símbolos `__imp_glfwXXX` (dllimport). Para
> linkar tudo estaticamente sem recompilar a raylib, o `Makefile` gera um pequeno
> *shim* (`glfw_imp_shim.c`, artefato de build ignorado pelo git) que redireciona
> esses símbolos para a GLFW estática de `libglfw3.a`.

## 📂 Estrutura

```
.
├── src/          # código-fonte C
├── assets/       # sprites, sons, fontes (pixel art)
├── docs/         # plano e fluxo de trabalho
├── Makefile      # build
└── README.md
```
