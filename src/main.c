#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "../include/render.h"
#include "../include/game.h"
#include "../include/puzzle.h"

int main() {
	initscr();

	if (has_colors() == FALSE) {
		endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

	// Inits
	start_color();
	init_graphics();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);
	srand(time(NULL));

	GameState* game = (GameState*)malloc(sizeof(GameState));
	if (!game) {
		endwin();
		printf("Failed to allocate gamestate");
		exit(1);
	}

	for (; ;) {
		// -- MENU LOOP --
		// Default menu state
		MenuState menu = {
			.section = MENU_DIFFICULTY,
			.diff = DIFF_MEDIUM,
			.size = 4
		};

		timeout(-1);
		render_menu(&menu); // Render before input
		int ch;
		while ((ch = getch()) != 'q') {
			if (handle_menu_input(&menu, ch)) break;
			render_menu(&menu);
		}
		if (ch == 'q') break; // q from menu exits program
		
		// Remove menu screen
		clear();
		refresh();
		
		Cursor cursor = {0, 0};
		game->size = menu.size;
		InputMode mode = MODE_NORMAL;
		bool go_menu = false;
		
		generate_puzzle(game);

		// Game loop
		timeout(1000); // Enable timer loop
		render(game, &cursor, &mode);
		while ((ch = getch()) != 'q') {
			if (check_win(game)) {
				render_win_screen(game);
				timeout(-1); // Block timer
				ch = getch();
				if (ch == 'q') {go_menu = true; break;}
				if (ch == 'r') {
					game->size = menu.size;
					generate_puzzle(game);
					cursor = (Cursor){0, 0};
					mode = MODE_NORMAL;
					timeout(1000); // Enable timer loop
				}
			} else {
				if (ch != ERR) {
					handle_input(game, &cursor, &mode, ch);
				}
				render(game, &cursor, &mode);
			}
		}

		if (ch == 'q') go_menu = true;
		if (!go_menu) break;

		clear();
		refresh();
	}


	free(game);
	endwin();
	return 0;
}
