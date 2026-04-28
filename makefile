# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Iinclude
LIBS    = -lncurses

# Target executable
TARGET  = kenken

# Source and object files
SRC     = src/main.c \
          src/render.c \
          src/game.c \
          src/puzzles.c
OBJ     = $(SRC:.c=.o)

# Install prefix (override with: make install PREFIX=/usr)
PREFIX  ?= /usr/local
BINDIR  = $(PREFIX)/bin
MANDIR  = $(PREFIX)/share/man/man6

# Default rule
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install rule
install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)/$(TARGET)
	@echo "Installed to $(BINDIR)/$(TARGET)"

# Uninstall rule
uninstall:
	rm -f $(BINDIR)/$(TARGET)
	@echo "Uninstalled $(TARGET)"

# Debug build with address sanitizer
debug: CFLAGS += -g -fsanitize=address
debug: LIBS   += -fsanitize=address
debug: $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all install uninstall debug clean
