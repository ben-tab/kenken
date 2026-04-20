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

void init_graphics() {
	// Init color pairs
	init_color(COLOR_MAGENTA, 500, 500, 500);
	init_pair(1, COLOR_GREEN, COLOR_BLACK); // ADD
	init_pair(2, COLOR_RED, COLOR_BLACK); // SUB
	init_pair(3, COLOR_BLUE, COLOR_BLACK); // MUL
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); // DIV
	init_pair(5, COLOR_WHITE, COLOR_BLACK); // EQL
	init_pair(6, COLOR_BLACK, COLOR_WHITE); // Highlight selected
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA); // Highlight grid
}

void render_board(GameState* game, Cursor* cursor, int bh, int bw, int boy, int box) {
	// Draw game box
	WINDOW* board = newwin(bh, bw, boy, box);

	box(board, 0, 0);

	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE; j++) {
			Cage* cage = game->cells[i][j].cage;
			int color = get_color(cage->op);

			int screen_y = (i*2);
			int screen_x = (j*4)+1;

			// Highlight selected cell
			int pair;
			if (cursor->row == i && cursor->col == j) {
				pair = 6;
			} else if (cage->cage_id == game->cells[cursor->row][cursor->col].cage->cage_id) {
				pair = 7;	
			} else {
				pair = color;
			}

			wattron(board, COLOR_PAIR(pair));

			int val = game->grid[i][j];
			if (val != 0) {
				mvwprintw(board, screen_y+1, screen_x+1, "%d", val);
			} else {
				mvwprintw(board, screen_y+1, screen_x+1, " ");
			}
			wattroff(board, COLOR_PAIR(pair));
			
		}
	}

	wrefresh(board);
	delwin(board);
}

void render_info(GameState* game, Cursor* cursor, int ih, int iw, int ioy, int iox) {
	Cell* cell = &game->cells[cursor->row][cursor->col];
	Cage* cage = cell->cage;

	WINDOW* info = newwin(ih, iw, ioy, iox);

	box(info, 0, 0);

	// Panel content
    attron(COLOR_PAIR(5));
    mvprintw(ioy + 1, iox+1, "Pos:    [%d, %d]", cursor->row, cursor->col);
    mvprintw(ioy + 2, iox+1, "Value:  %d", game->grid[cursor->row][cursor->col]);
    mvprintw(ioy + 3, iox+1, "Cage:   #%d", cage->cage_id);
    mvprintw(ioy + 4, iox+1, "Op:     %c", (char)cage->op);
    mvprintw(ioy + 5, iox+1, "Target: %d", cage->target);
    mvprintw(ioy + 6, iox+1, "Size:   %d", cage->size);
    attroff(COLOR_PAIR(5));

    wrefresh(info);
    delwin(info);
}

void render(GameState* game, Cursor* cursor) {
	// Get sizes;
	int term_rows, term_cols;
	getmaxyx(stdscr, term_rows, term_cols);
	
	int board_h = 2*(SIZE)+1;
	int board_w = 4*(SIZE)+1;
	int board_offset_y = (term_rows - board_h)/4;
	int board_offset_x = (term_rows - board_w) / 2;

	int info_h = board_h;
	int info_w = 20;
	int info_offset_y = board_offset_y;
	int info_offset_x = board_offset_x + board_w + 4;


	render_board(game, cursor, board_h, board_w, board_offset_y, board_offset_x);
	render_info(game, cursor, info_h, info_w, info_offset_y, info_offset_x);
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
    mvprintw(mid_y, mid_x-10, "Solved in %02d:%02d", mins, secs);
    mvprintw(mid_y+2, mid_x-11, "R to play again, Q to quit");
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
