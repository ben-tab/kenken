#include <ncurses.h>
#include <stdio.h>
#include "../include/render.h"

/*
	Get color based on cage operation
*/
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

/*
	Initialize color pairs	
*/
void init_graphics() {
	// Init color pairs
	init_color(COLOR_MAGENTA, 700, 700, 700); // Doesn't work on my terminal, maybe on some it will idk
	init_pair(1, COLOR_GREEN, COLOR_BLACK); // ADD
	init_pair(2, COLOR_RED, COLOR_BLACK); // SUB
	init_pair(3, COLOR_BLUE, COLOR_BLACK); // MUL
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); // DIV
	init_pair(5, COLOR_WHITE, COLOR_BLACK); // EQL
	init_pair(6, COLOR_BLACK, COLOR_WHITE); // Highlight selected
	init_pair(7, COLOR_BLACK, COLOR_MAGENTA); // Highlight grid
}

/*
	Render board
*/
void render_board(GameState* game, Cursor* cursor, int bh, int bw, int by, int bx) {
	// Draw game box
	WINDOW* board = newwin(bh, bw, by, bx);
	werase(board);

	box(board, 0, 0);

	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			Cage* cage = game->cells[i][j].cage;
			int color = get_color(cage->op);
			int screen_y = (i*CELL_H)+1; // +1 for box border
			int screen_x = (j*CELL_W)+1;

			// Highlight selected cell
			int pair;
			if (cursor->row == i && cursor->col == j) {
				pair = 6;
			} else if (cage->cage_id == game->cells[cursor->row][cursor->col].cage->cage_id) {
				pair = 7;	
			} else {
				pair = color;
			}

			// Fill all 3 rows of cell with the color
			for (int dy=0; dy<CELL_H; dy++) {
				for (int dx=0; dx<CELL_W; dx++) {
					mvwaddch(board, screen_y+dy, screen_x+dx, ' ' | COLOR_PAIR(pair));
				}
			}

			// Find top-left-most cell in cage
			int label_row = cage->coords[0][0];
			int label_col = cage->coords[0][1];
			for (int k=1; k<cage->size; k++) {
				int r = cage->coords[k][0];
				int c = cage->coords[k][1];
				if (r < label_row || (r == label_row && c < label_col)) {
					label_row = r;
					label_col = c;
				}
			}

			// Cage label in top left cell of cage
			bool is_label = (label_row == i && label_col == j);
			if (is_label) {
				wattron(board, COLOR_PAIR(pair) | A_BOLD);
				mvwprintw(board, screen_y, screen_x, "%d%c", cage->target, (char)cage->op);
				wattroff(board, COLOR_PAIR(pair) | A_BOLD);
			}

			// Player value
			int val = game->grid[i][j];
			if (val != 0) {
				wattron(board, COLOR_PAIR(pair) | A_BOLD);
				mvwprintw(board, screen_y+CELL_H/2, screen_x+CELL_W/2, "%d", val);
				wattroff(board, COLOR_PAIR(pair) | A_BOLD);
			} else {
				// Draw notes in 3x2 grid in cell, up to 6 notes shown as small numbers
				int note_positions[6][2] = {{0, 0}, {0, 2}, {0, 4}, {1, 0}, {1, 2}, {1, 4}};
				wattron(board, COLOR_PAIR(pair));
				for (int k=0; k<game->size && k<6; k++) {
					if (game->notes[i][j][k]) {
						int ny = screen_y + note_positions[k][0] + 1;
						int nx = screen_x + note_positions[k][1] + 1;
						mvwprintw(board, ny, nx, "%d", k+1);
					}
				}
				wattroff(board, COLOR_PAIR(pair));
			}
			
			
		}
	}

	wrefresh(board);
	delwin(board);
}

/*
	Render timer for game
*/
void render_timer(GameState* game, int board_offset_y, int board_offset_x, int board_w) {
	int elapsed = (int)(time(NULL) - game->start_time);
	int mins = elapsed / 60;
	int secs = elapsed % 60;
	
	int timer_w = 8;
	int timer_x = board_offset_x + (board_w/2)-(timer_w/2);
	
	WINDOW* timer = newwin(1, timer_w, board_offset_y-2, timer_x);
	werase(timer);

	wattron(timer, COLOR_PAIR(5));
	mvwprintw(timer, 0, 0, "%02d:%02d", mins, secs);
	wattroff(timer, COLOR_PAIR(5));

	wrefresh(timer);
	delwin(timer);
}

/*
	Main render function that calls other render functions
*/
void render(GameState* game, Cursor* cursor, InputMode* mode) {
	// Get sizes;
	int term_rows, term_cols;
	getmaxyx(stdscr, term_rows, term_cols);
	
	int board_h = CELL_H*(game->size)+2;
	int board_w = CELL_W*(game->size)+2;
	int board_offset_y = (term_rows - board_h)/2;
	int board_offset_x = (term_cols - board_w)/2;

	if (board_offset_y < 0) board_offset_y = 0;
	if (board_offset_x < 0) board_offset_x = 0;
	
	render_board(game, cursor, board_h, board_w, board_offset_y, board_offset_x);
	render_timer(game, board_offset_y, board_offset_x, board_w);

	attron(COLOR_PAIR(*mode == MODE_NOTE ? 4 : 5));
	mvprintw(board_offset_y+board_h+1, board_offset_x, 
	*mode == MODE_NOTE ? "MODE: Note [n]  " : "MODE: Normal [n]");
	attroff(COLOR_PAIR(*mode == MODE_NOTE ? 4 : 5));
	refresh();
}

/*
	Win screen that shows time
*/
void render_win_screen(GameState* game) {
    clear();

    int term_rows, term_cols;
   	getmaxyx(stdscr, term_rows, term_cols);
   	int mid_y = term_rows / 2;
   	int mid_x = term_cols / 2;

    int elapsed = (int)(time(NULL) - game->start_time);
    int mins = elapsed / 60;
    int secs = elapsed % 60;

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(mid_y - 2, mid_x - 4, "YOU WIN!");
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(5));
    mvprintw(mid_y, mid_x-10, "Solved in %02d:%02d", mins, secs);
    mvprintw(mid_y+2, mid_x-11, "R to play again, Q to quit");
    attroff(COLOR_PAIR(5));

    refresh();
}

/*
	Render menu screen
*/
int render_menu(MenuState* menu) {
	clear();

	int term_rows, term_cols;
	getmaxyx(stdscr, term_rows, term_cols);
	int mid_y = term_rows / 2;
	int mid_x = term_cols / 2;

	// ASCII art title 
	attron(COLOR_PAIR(3) | A_BOLD);
	mvprintw(mid_y-12, mid_x-13, "  _  _____ _   _ _  _____ _   _ ");
	mvprintw(mid_y-11, mid_x-13, " | |/ / __| \\ | | |/ / __| \\ | |");
	mvprintw(mid_y-10, mid_x-13, " | ' <| _||  \\| | ' <| _||  \\| |");
	mvprintw(mid_y-9,  mid_x-13, " |_|\\_\\___|_|\\__|_|\\_\\___|_|\\__|");
	attroff(COLOR_PAIR(3) | A_BOLD);

	// Difficulty
	attron(COLOR_PAIR(5) | (menu->section == MENU_DIFFICULTY ? A_BOLD : 0));
	mvprintw(mid_y-6, mid_x-16, "DIFFICULTY:");
	attroff(COLOR_PAIR(5) | A_BOLD);

	const char* diff_labels[] = {"EASY", "MEDIUM", "HARD"};
	int diff_colors[] = {1, 4, 2}; // Color pairs for each difficulty
	int diff_offsets[] = {0, 9, 20}; // Manual offset based on label width

	for (int i=0; i<DIFF_COUNT; i++) {
		bool is_selected = (menu->diff == i);
		bool is_focused = (menu->section == MENU_DIFFICULTY);
		if (is_selected) {
			attron(COLOR_PAIR(diff_colors[i]) | A_BOLD | (is_focused ? A_REVERSE : 0));
		} else {
			attron(COLOR_PAIR(5));
		}
		mvprintw(mid_y-6, mid_x-3+diff_offsets[i], "%s%s%s", 
		is_selected ? "[" : " ",
		diff_labels[i],
		is_selected ? "]" : " ");
		attroff(COLOR_PAIR(diff_colors[i]) | A_BOLD | A_REVERSE | COLOR_PAIR(5));
	}

	// Size
	attron(COLOR_PAIR(5) | (menu->section == MENU_SIZE ? A_BOLD : 0));
	mvprintw(mid_y-2, mid_x-20, "SIZE:");
	attroff(COLOR_PAIR(5) | A_BOLD);

	for (int i=MIN_SIZE; i<=MAX_SIZE; i++) {
		bool is_selected = (menu->size == i);
		bool is_focused = (menu->section == MENU_SIZE);
		if (is_selected) {
			attron(COLOR_PAIR(3) | A_BOLD | (is_focused ? A_REVERSE : 0));
		} else {
			attron(COLOR_PAIR(5));
        }
		mvprintw(mid_y-2, mid_x-13+((i-3)*6), "%s%dx%d%s",
		is_selected ? "[" : " ",
		i, i,
		is_selected ? "]" : " ");
		attroff(COLOR_PAIR(3) | A_BOLD | A_REVERSE | COLOR_PAIR(5));
	}

	// Play
	bool play_focused = (menu->section == MENU_PLAY);
	attron(COLOR_PAIR(1) | A_BOLD | (play_focused ? A_REVERSE : 0));
	mvprintw(mid_y+2, mid_x-2, "[ PLAY ]");
	attroff(COLOR_PAIR(1) | A_BOLD | A_REVERSE);

	refresh();
	return 0;
}

/*
	Menu input handling
*/
int handle_menu_input(MenuState* menu, int ch) {
	switch (ch) {
		case KEY_UP: case 'k': menu->section = (menu->section-1+MENU_SECTION_COUNT) % MENU_SECTION_COUNT; break;
		case KEY_DOWN: case 'j': menu->section = (menu->section+1) % MENU_SECTION_COUNT; break;
		case KEY_LEFT: case 'h':
			if (menu->section == MENU_DIFFICULTY) menu->diff = (menu->diff-1+DIFF_COUNT) % DIFF_COUNT;
			else if (menu->section == MENU_SIZE) menu->size = (menu->size-1 < MIN_SIZE) ? MAX_SIZE : menu->size-1;
			break;
		case KEY_RIGHT: case 'l':
			if (menu->section == MENU_DIFFICULTY) menu->diff = (menu->diff+1) % DIFF_COUNT;
			else if (menu->section == MENU_SIZE) menu->size = (menu->size+1 > MAX_SIZE) ? MIN_SIZE : menu->size+1;
			break;
		case '\n': case KEY_ENTER:
			if (menu->section == MENU_PLAY) return 1;
			menu->section = (menu->section + 1) % MENU_SECTION_COUNT;
			break;
		default: break;
	}
	return 0;
}

/*
	Input handling
*/
void handle_input(GameState* game, Cursor* cursor, InputMode* mode, int ch) {
	switch (ch) {
		// Movement
		case KEY_UP: case 'k': cursor->row = (cursor->row-1+game->size) % game->size; break;
		case KEY_DOWN: case 'j': cursor->row = (cursor->row+1) % game->size; break;
		case KEY_LEFT: case 'h': cursor->col = (cursor->col-1+game->size) % game->size; break;
		case KEY_RIGHT: case 'l': cursor->col = (cursor->col+1) % game->size; break;

		// Toggle note mode
		case 'n':
			*mode = (*mode == MODE_NOTE) ? MODE_NORMAL : MODE_NOTE;
			break;

		// Num input
		case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
			int num = ch - '0';
			if (num > game->size) break;

			if (*mode == MODE_NOTE) {
				int* note = &game->notes[cursor->row][cursor->col][num-1];
				*note = !*note;
			} else {
				game->grid[cursor->row][cursor->col] = num;

				// Clear notes when value entered
				for (int i=0; i<game->size; i++) {
					game->notes[cursor->row][cursor->col][i] = 0;
				}
			}
			break;
		}

		// Clear cell
		case KEY_BACKSPACE: case 'd': case '0':
			game->grid[cursor->row][cursor->col] = 0;
			for (int i=0; i<game->size; i++) {
				game->notes[cursor->row][cursor->col][i] = 0;
			}
			break;

		// Show cell
		case 'c':
			game->grid[cursor->row][cursor->col] = game->solution[cursor->row][cursor->col];
			for (int i=0; i<game->size; i++) {
				game->notes[cursor->row][cursor->col][i] = 0;
			}
			break;

		default: break;
	}
}
