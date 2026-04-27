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

	// Default menu state
	MenuState menu = {
		.section = MENU_DIFFICULTY,
		.diff = DIFF_MEDIUM,
		.size = 4
	};

	// Menu loop
	int ch;
	render_menu(&menu); // Render before input
	while ((ch = getch()) != 'q') {
		if (handle_menu_input(&menu, ch)) break;
		render_menu(&menu);
	}

	// Remove menu screen
	clear();
	refresh();
	
	GameState game;
	Cursor cursor = {0, 0};
	game.size = menu.size;

	// Load prebuilt 4x4 puzzle (change later when generating puzzles)
	load_puzzle(&game);

	// Game loop
	render(&game, &cursor);
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
