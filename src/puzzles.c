#include "../include/game.h"

#define CAGE_COUNT 7

void clear_grid(GameState* game) {
	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE; j++) {
			game->grid[i][j] = 0;
		}
	}	
}

void load_puzzle(GameState* game) {
	clear_grid(game);
	game->start_time = time(NULL);
	
	// Hardcoded solution currently
	int sol[SIZE][SIZE] = {
		{4, 2, 1, 3},
		{2, 3, 4, 1},
		{3, 1, 2, 4},
		{1, 4, 3, 2}
	};
	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE; j++) {
			game->solution[i][j] = sol[i][j];
		}
	}

	game->cage_count = CAGE_COUNT;

	// Coords (y, x)
	game->cages[0] = (Cage){
		.coords = {{0, 0}, {1, 0}, {1, 1}},
		.size = 3,
		.op = MUL,
		.target = 24,
		.cage_id = 1
	};
	game->cages[1] = (Cage){
		.coords = {{0, 1}, {0, 2}},
		.size = 2,
		.op = ADD,
		.target = 3,
		.cage_id = 2
	};
	game->cages[2] = (Cage){
		.coords = {{0, 3}, {1, 2}, {1, 3}},
		.size = 3,
		.op = MUL,
		.target = 12,
		.cage_id = 3
	};
	game->cages[3] = (Cage){
		.coords = {{2, 0}, {2, 1}},
		.size = 2,
		.op = SUB,
		.target = 2,
		.cage_id = 4
	};
	game->cages[4] = (Cage){
		.coords = {{2, 2}, {2, 3}},
		.size = 2,
		.op = DIV,
		.target = 2,
		.cage_id = 5
	};
	game->cages[5] = (Cage){
		.coords = {{3, 0}},
		.size = 1,
		.op = EQL,
		.target = 1,
		.cage_id = 6
	};
	game->cages[6] = (Cage){
		.coords = {{3, 1}, {3, 2}, {3, 3}},
		.size = 3,
		.op = MUL,
		.target = 24,
		.cage_id = 7
	};


	// Link cells to cage
	for (int i=0; i<game->cage_count; i++) {
		Cage* cage = &game->cages[i];
		for (int j=0; j<cage->size; j++) {
			int row = cage->coords[j][0];
			int col = cage->coords[j][1];
			game->cells[row][col].cage = cage;
			game->cells[row][col].row = row;
			game->cells[row][col].col = col;
		}
	}

}
