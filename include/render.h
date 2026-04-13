#ifndef UI_H
#define UI_H

#include "game.h"

#define CELL_W 6
#define CELL_H 3

typedef struct {
	int row;
	int col;
} Cursor;

void render(GameState* game, Cursor* cursor);
void render_win_screen(GameState* game);
void handle_input(GameState* game, Cursor* cursor, int ch);

#endif
