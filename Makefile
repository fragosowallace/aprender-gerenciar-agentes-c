# Makefile — Vampire Survivors em C com raylib (MSYS2 UCRT64)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Isrc
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
SRC     = $(wildcard src/*.c)
TARGET  = game.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
