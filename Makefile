# Makefile — Vampire Survivors em C com raylib (MSYS2 UCRT64)
#
# Linkagem ESTÁTICA: o game.exe embute a raylib, a GLFW e o runtime do gcc
# (libgcc / libwinpthread), tornando-se autossuficiente. Assim ele roda fora
# do terminal MSYS2 (ex.: duplo-clique no Explorer) sem depender de
# libraylib.dll, glfw3.dll, libgcc_s_seh-1.dll nem libwinpthread-1.dll.

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Isrc
SRC     = $(wildcard src/*.c)
TARGET  = game.exe

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
