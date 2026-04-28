#ifndef GAME_H
#define GAME_H
#include "defs.h"

// Chances for cage size
#define SINGLE_CELL_CHANCE 5
#define DOUBLE_CELL_CHANCE 35
#define TRIPLE_CELL_CHANCE 40
#define QUADRUPLE_CELL_CHANCE 20

/* Checks if the player's current grid matches the solution.
 * Returns true if every cell matches, false if any cell differs or is empty.
 *
 * game: pointer to current game state */
bool check_win(GameState* game);

/* Sets all cells in the player grid to 0 (empty).
 * Does not affect the solution or cage data.
 *
 * game: pointer to current game state */
void clear_grid(GameState* game);

/* Fisher-Yates shuffle of an integer array in place.
 *
 * arr:  pointer to the array to shuffle
 * size: number of elements in the array */
void shuffle(int* arr, int size);

/* Returns a random cage size based on weighted probabilities.
 * Weights are defined in defs.h as:
 *   SINGLE_CELL_CHANCE, DOUBLE_CELL_CHANCE, TRIPLE_CELL_CHANCE
 * The remaining probability produces a 4-cell cage. */
int weighted_random();

/* Generates a randomized latin square and stores it in game->solution.
 * Starts from a base shift pattern then shuffles rows, columns, and
 * symbols independently to produce a unique valid solution each time.
 * game->size must be set before calling.
 *
 * game: pointer to current game state */
void generate_latin_square(GameState* game);

/* Assigns an operation and target value to a cage based on its cells.
 * Single-cell cages always use EQL. Two-cell cages may use any operation
 * (DIV falls back to ADD if the division is not clean). Cages of 3 or more
 * cells only use ADD or MUL.
 *
 * game: pointer to current game state (reads solution values)
 * cage: pointer to the cage to assign */
void assign_cage_op(GameState* game, Cage* cage);

/* Generates a full puzzle by producing a latin square, randomly growing
 * cages from unassigned cells, assigning operations, and linking each
 * cell to its cage pointer. game->size must be set before calling.
 * Also sets game->start_time to the current time.
 *
 * game: pointer to current game state */
void generate_puzzle(GameState* game);

#endif
