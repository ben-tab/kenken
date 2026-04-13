#include <ncurses.h>
#include <stdio.h>
#include "../include/render.h"

int get_color(Operation op) {
    switch (op) {
        case ADD: return 1;
        case SUB: return 2;
        case MUL: return 3;
        case DIV: return 4;
        case EQL: return 5;
        default:  return 5;
    }
}

void render(GameState* game, Cursor* cursor) {
	clear();

	init_pair(1, COLOR_GREEN, COLOR_BLACK); // ADD
	init_pair(2, COLOR_RED, COLOR_BLACK); // SUB
	init_pair(3, COLOR_BLUE, COLOR_BLACK); // MUL
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); // DIV
	init_pair(5, COLOR_WHITE, COLOR_BLACK); // EQL

	int term_rows, term_cols;
	getmaxyx(stdscr, term_rows, term_cols);
	int board_h = SIZE * CELL_H;
	int board_w = SIZE * CELL_W;
	int offset_y = (term_rows - board_h) / 4;
	int offset_x = (term_rows - board_w) / 2;

	// draw outer box around the whole board
    for (int r = offset_y - 1; r <= offset_y + board_h; r++) {
        mvaddch(r, offset_x - 1,          ACS_VLINE);
        mvaddch(r, offset_x + board_w,    ACS_VLINE);
    }
    for (int c = offset_x - 1; c <= offset_x + board_w; c++) {
        mvaddch(offset_y - 1,         c,  ACS_HLINE);
        mvaddch(offset_y + board_h,   c,  ACS_HLINE);
    }
    // corners
    mvaddch(offset_y - 1,       offset_x - 1,       ACS_ULCORNER);
    mvaddch(offset_y - 1,       offset_x + board_w, ACS_URCORNER);
    mvaddch(offset_y + board_h, offset_x - 1,       ACS_LLCORNER);
    mvaddch(offset_y + board_h, offset_x + board_w, ACS_LRCORNER);

    // add just before the cell drawing loop
   	mvprintw(offset_y - 3, offset_x + board_w / 2 - 3, "KENKEN");

	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE; j++) {
			Cage* cage = game->cells[i][j].cage;
			int color = get_color(cage->op);
			
			int screen_y = offset_y+i*CELL_H;
			int screen_x = offset_x+j*CELL_W;

			// Highlight selected cell
			if (cursor->row == i && cursor->col == j) attron(A_REVERSE);

			attron(COLOR_PAIR(color));

			// Top-left cell of cage shows target+op
            bool is_label = (cage->coords[0][0] == i && cage->coords[0][1] == j);
            if (is_label) {
                mvprintw(screen_y, screen_x, "%d%c   ", cage->target, (char)cage->op);
            } else {
                mvprintw(screen_y, screen_x, "      ");
			}
			
            // middle row shows the player's value
            int val = game->grid[i][j];
            if (val == 0) {
                mvprintw(screen_y + 1, screen_x, "  .   ");
            } else {
                mvprintw(screen_y + 1, screen_x, "  %d   ", val);
			}

            // bottom row empty (spacing)
            mvprintw(screen_y + 2, screen_x, "      ");
			
			attroff(COLOR_PAIR(color));
			attroff(A_REVERSE);
		}
	}

	refresh();
}

void render_win_screen(GameState* game) {
    clear();

    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    int elapsed = (int)(time(NULL) - game->start_time);
    int mins = elapsed / 60;
    int secs = elapsed % 60;

    int mid_y = term_rows / 2;
    int mid_x = term_cols / 2;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(mid_y - 2, mid_x - 4, "YOU WIN!");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(5));
    mvprintw(mid_y,     mid_x - 10, "Solved in %02d:%02d", mins, secs);
    mvprintw(mid_y + 2, mid_x - 11, "R to play again, Q to quit");
    attroff(COLOR_PAIR(5));

    refresh();
}

void handle_input(GameState* game, Cursor* cursor, int ch) {
	switch (ch) {
		// Movement
		case KEY_UP: case 'k': cursor->row = (cursor->row-1+SIZE) % SIZE; break;
		case KEY_DOWN: case 'j': cursor->row = (cursor->row+1) % SIZE; break;
		case KEY_LEFT: case 'h': cursor->col = (cursor->col-1+SIZE) % SIZE; break;
		case KEY_RIGHT: case 'l': cursor->col = (cursor->col+1) % SIZE; break;

		// Num input
		case '1': case '2': case '3': case '4':
			game->grid[cursor->row][cursor->col] = ch - '0';
			break;

		// Clear cell
		case KEY_BACKSPACE: case 'd':
			game->grid[cursor->row][cursor->col] = 0;
			break;

		default: break;
	}
}
