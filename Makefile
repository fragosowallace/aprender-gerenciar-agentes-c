# Makefile — Vampire Survivors em C com raylib (MSYS2 UCRT64)
#
# Linkagem ESTÁTICA: o game.exe embute a raylib, a GLFW e o runtime do gcc
# (libgcc / libwinpthread), tornando-se autossuficiente. Assim ele roda fora
# do terminal MSYS2 (ex.: duplo-clique no Explorer) sem depender de
# libraylib.dll, glfw3.dll, libgcc_s_seh-1.dll nem libwinpthread-1.dll.

# ---------------------------------------------------------------------------
# Shell das receitas — portabilidade entre terminais
# ---------------------------------------------------------------------------
# As receitas abaixo usam utilitários Unix (rm, nm, grep, sed, sort, while
# read). Quando o `mingw32-make` é chamado do PowerShell ou do cmd, o make
# executaria as receitas via `cmd.exe`, que não conhece esses comandos e
# falha com "CreateProcess ... (e=2)". Para que a build funcione de QUALQUER
# terminal, forçamos o make a usar o `sh` do MSYS2 como shell das receitas.
#
# Detectamos o caminho do sh: dentro do MSYS2 (onde `sh` já está no PATH)
# usamos o próprio `sh`; caso contrário caímos no caminho absoluto padrão do
# MSYS2. Sobrescreva na linha de comando se instalou em outro lugar, ex.:
#   mingw32-make SH=D:/msys64/usr/bin/sh.exe
# Raiz da instalação do MSYS2. Sobrescreva na linha de comando se instalou em
# outro lugar, ex.:  mingw32-make MSYS2=D:/msys64
MSYS2 ?= C:/msys64

SH ?= $(if $(wildcard $(MSYS2)/usr/bin/sh.exe),$(MSYS2)/usr/bin/sh.exe,/usr/bin/sh)
SHELL := $(SH)
.SHELLFLAGS := -c

# PATH — dois problemas resolvidos de uma vez:
#
# (1) Para que o make consiga LANÇAR o próprio sh.exe, o Windows precisa achar
#     a DLL de que ele depende (msys-2.0.dll, em $(MSYS2)/usr/bin). Quando o
#     make roda do PowerShell/cmd esse diretório NÃO está no PATH, o sh.exe
#     falha ao carregar e o make cai de volta no cmd.exe — reproduzindo o erro
#     "CreateProcess ... (e=2)" da issue. Por isso o PATH precisa conter os
#     diretórios do MSYS2 em estilo NATIVO do Windows (com ';' e 'C:/...').
#
# (2) Dentro das receitas, o sh precisa achar rm/nm/grep/sed/sort (usr/bin) e
#     gcc/objdump (ucrt64/bin). O sh do MSYS2 converte automaticamente este
#     PATH nativo para o formato POSIX que ele usa internamente.
#
# Prefixamos os dois diretórios e mantemos o PATH herdado ao final, então o
# caso que já funcionava no terminal MSYS2 continua intacto.
export PATH := $(MSYS2)/usr/bin;$(MSYS2)/ucrt64/bin;$(PATH)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Isrc
SRC     = $(wildcard src/*.c)
TARGET  = game.exe

# Alvo padrão: sem isto, o primeiro alvo do arquivo (a regra do shim, mais
# abaixo) seria construído por padrão, e `make` sem argumento NÃO geraria o
# game.exe. Fixamos explicitamente para evitar depender da ordem das regras.
.DEFAULT_GOAL := all

# Arquivos estáticos (.a) resolvidos de forma portável pelo próprio gcc.
RAYLIB_A = $(shell $(CC) -print-file-name=libraylib.a)
GLFW_A   = $(shell $(CC) -print-file-name=libglfw3.a)

# -static embute libgcc/libwinpthread (runtime do gcc) no exe.
# A raylib e a GLFW entram pelos .a explícitos acima; o restante são libs
# de sistema do Windows (OpenGL/GDI/WinMM), que ficam sempre disponíveis.
LDFLAGS = -static -lopengl32 -lgdi32 -lwinmm

# ---------------------------------------------------------------------------
# Shim de importação da GLFW
# ---------------------------------------------------------------------------
# O pacote libraylib.a do MSYS2 é compilado com -DGLFW_DLL, então ele chama a
# GLFW via símbolos "__imp_glfwXXX" (dllimport), esperando glfw3.dll em runtime.
# Para linkar TUDO estaticamente sem recompilar a raylib, geramos um pequeno
# objeto que aponta cada "__imp_glfwXXX" para a função "glfwXXX" real vinda de
# libglfw3.a. Com isso o exe não depende mais de glfw3.dll.
GLFW_SHIM = glfw_imp_shim.c

$(GLFW_SHIM):
	@echo "/* auto-gerado: aliases __imp_glfwXXX -> glfwXXX para linkagem estatica */" > $@
	@nm "$(RAYLIB_A)" | grep "U __imp_glfw" | sed 's/.*U __imp_//' | sort -u | \
	while read s; do \
		echo "extern void $$s(void);" >> $@; \
		echo "void (*__imp_$$s)(void) = $$s;" >> $@; \
	done

all: $(TARGET)

$(TARGET): $(SRC) $(GLFW_SHIM)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(GLFW_SHIM) "$(RAYLIB_A)" "$(GLFW_A)" $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(GLFW_SHIM)

.PHONY: all run clean
