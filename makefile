# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Iinclude  # -Iinclude adds a header directory if you have one
LIBS = -lncurses

# Target Executable Name
TARGET = kenken

# Source Files
SRC = src/main.c \
      src/render.c \
      src/game.c \
      src/puzzles.c

# Object Files (This converts .c filenames to .o filenames)
OBJ = $(SRC:.c=.o)

# Default Rule: Build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

# Rule for compiling .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Rule: Remove compiled files
clean:
	rm -f $(TARGET) $(OBJ)
