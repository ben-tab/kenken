#ifndef UI_H
#define UI_H

#include "defs.h"

/* Cell dimensions in terminal characters
 * CELL_W: columns per cell, includes padding for label and value display as well as border
 * CELL_H: rows per cell, top row is cage label, middle is value and bottom is padding + border */
#define CELL_W 11
#define CELL_H 5

// Sections in menu
typedef enum {
    MENU_DIFFICULTY,
    MENU_SIZE,
    MENU_PLAY,
    MENU_SECTION_COUNT
} MenuSection;

// Difficulty
typedef enum {
    DIFF_EASY,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_COUNT
} Difficulty;

// Track current state of menu
typedef struct {
	MenuSection section;  // Section that is focused
	Difficulty diff; // Selected difficulty
	int size;
} MenuState;

typedef enum {
	MODE_NORMAL, // Regular input mode
	MODE_NOTE // Write notes in cells
} InputMode;

// Track players current selected cell on the grid (0, 0) is top left cell in grid
typedef struct {
	int row;
	int col;
} Cursor;

/* Initializes ncurses color pairs used throughout the UI.
 * Must be called once after start_color() in main before any rendering.
 * Color pairs:
 *   1 = GREEN   on BLACK   (ADD cages)
 *   2 = RED     on BLACK   (SUB cages)
 *   3 = BLUE    on BLACK   (MUL cages)
 *   4 = YELLOW  on BLACK   (DIV cages)
 *   5 = WHITE   on BLACK   (EQL cages / general text)
 *   6 = BLACK   on WHITE   (selected cell highlight)
 *   7 = BLACK   on MAGENTA (same-cage highlight) */
void init_graphics();

/* Renders the game board into a centered ncurses window.
 * Each cell is drawn as a CELL_W x CELL_H block colored by its cage operation.
 * The selected cell (cursor) is highlighted white, and all other cells in
 * the same cage are highlighted magenta. The cage label (e.g. "6*") is
 * drawn in the top-left cell of each cage. The player's current value is
 * drawn in the center of each cell, or '.' if empty.
 *
 * game:   pointer to current game state (grid values, cage info, cell metadata)
 * cursor: pointer to the player's current cursor position
 * bh:     total board window height in terminal rows    (CELL_H * SIZE + 2)
 * bw:     total board window width in terminal columns  (CELL_W * SIZE + 2)
 * boy:    board window top-left row    (y offset from top of terminal)
 * box:    board window top-left column (x offset from left of terminal) */
void render_board(GameState* game, Cursor* cursor, int bh, int bw, int boy, int box);

/* Renders a timer above the board showing elapsed time since puzzle start.
 * Positioned centered above the board using the board offset and width.
 *
 * game:            pointer to current game state (reads start_time)
 * board_offset_y:  top row of the board window
 * board_offset_x:  left column of the board window
 * board_w:         width of the board window in columns */
void render_timer(GameState* game, int board_offset_y, int board_offset_x, int board_w);

/* Main render entry point. Calculates terminal dimensions and centering
 * offsets, then calls render_board and render_info to draw the full UI.
 * Both the board and info panel are treated as one unit when centering
 * so they stay visually balanced regardless of terminal size.
 * Should be called once per iteration of the main game loop.
 *
 * game:   pointer to current game state
 * cursor: pointer to the player's current cursor position
 * mode:   pointer to the player's current toggled mode */
void render(GameState* game, Cursor* cursor, InputMode* mode);

/* Renders the main menu with difficulty, size and play sections
 * Focused section is bolded so if you're on size the size section will be bolded
 * Selected options are highlighted
 * Returns 0 if everything went good
 *
 * menu: pointer to current menu state
 */
int render_menu(MenuState* menu);

/* Renders a win screen centered on the terminal showing "YOU WIN!" and
 * the player's solve time in MM:SS format. Waits for the caller to handle
 * the next keypress — this function only draws and refreshes, it does not
 * call getch(). The caller (main loop) is responsible for handling
 * 'r' (replay) and 'q' (quit) after this is shown.
 *
 * game: pointer to current game state (used to read start_time for elapsed calc) */
void render_win_screen(GameState* game);

/* Handles a single menu keypress and updates menu state.
 * Up/down moves between sections. Left/right changes the selected value
 * within the focused section. Enter on PLAY returns 1 to signal game start,
 * enter on other sections advances focus to the next section.
 *
 * menu: pointer to current menu state (mutated on any input)
 * ch:   keypress value returned by getch()
 * returns 1 if PLAY was confirmed, 0 otherwise */
int handle_menu_input(MenuState* menu, int ch);

/* Handles a single keypress and updates game or cursor state accordingly.
 * Movement: arrow keys and hjkl move the cursor, wrapping at grid edges.
 * Input:    '1'-'n' sets the value of the cell under the cursor.
 * Clear:    backspace, 'd', or '0' sets the cell under the cursor to 0 (empty).
 * Check:    'c' to check cell and input correct value
 * Does NOT check for win conditions — that is the caller's responsibility.
 *
 * game:   pointer to current game state (grid is mutated on number input)
 * cursor: pointer to cursor (mutated on movement keys)
 * mode:   pointer to current mode (normal or note)
 * ch:     the keypress value returned by getch() */
void handle_input(GameState* game, Cursor* cursor, InputMode* mode, int ch);

#endif
