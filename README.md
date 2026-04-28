# KenKen

A terminal-based KenKen puzzle game built with ncurses.

![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-blue)

## What is KenKen?

KenKen is a math puzzle played on an NxN grid. Fill the grid with numbers
1 to N so that:
- Each number appears exactly once in every row and column
- The numbers in each outlined cage produce the given target using the
  cage's operation (+, -, *, /)

## Controls

| Key                   | Action          |
|-----------------------|-----------------|
| `hjkl` / arrows       | Move cursor     |
| `1`-`9`               | Enter a value   |
| `d` / backspace / `0` | Clear a cell    |
| `q`                   | Quit            |
| `r`                   | New puzzle      |
| `c`                   | Reveal cell     |

## Dependencies

You need the ncurses development library installed to build:

```bash
# Ubuntu / Debian
sudo apt install libncurses-dev

# Fedora
sudo dnf install ncurses-devel

# Arch
sudo pacman -S ncurses

# macOS (via Homebrew)
brew install ncurses
```

## Building and Installing

```bash
git clone https://github.com/you/kenken
cd kenken
make
sudo make install
```

Then just run:

```bash
kenken
```

To uninstall:

```bash
sudo make uninstall
```

## Building from Source (without installing)

```bash
make
./kenken
```

## Debug Build

```bash
make debug
./kenken
```

Compiles with `-fsanitize=address` and debug symbols for catching memory issues.

## Grid Sizes

Select from 3x3 up to 9x9 in the main menu. Larger grids are harder.

## Difficulty

IN PROGRESS

| Difficulty | Effect                                      |
|------------|---------------------------------------------|
| Easy       |                    N/A                      |
| Medium     |                    N/A                      |
| Hard       |                    N/A                      |


