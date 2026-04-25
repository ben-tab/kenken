#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "../include/render.h"
#include "../include/puzzle.h"

int main() {
	initscr();

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	
	start_color();
	init_graphics();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);
	
	GameState game;
	Cursor cursor = {0, 0};

	// Load prebuilt 4x4 puzzle (change later when generating puzzles)
	load_puzzle(&game);

	// Game loop
	int ch;
	while ((ch = getch()) != 'q') {
		if (check_win(&game)) {
			render_win_screen(&game);
			ch = getch();
			if (ch == 'q') break;
			if (ch == 'r') {
				load_puzzle(&game);
				cursor = (Cursor){0, 0};
			}
		} else {
			handle_input(&game, &cursor, ch);
			render(&game, &cursor);
		}
	}
	
	endwin();
	return 0;
}
