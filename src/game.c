#include <stdlib.h>
#include "../include/game.h"

// Basic check win function
bool check_win(GameState* game) {
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			if (game->grid[i][j] != game->solution[i][j]) {
				return false;
			}
		}
	}
	return true;
}

// Clear game grid
void clear_grid(GameState* game) {
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			game->grid[i][j] = 0;
		}
	}	
}

// Helper 
void shuffle(int* arr, int size) {
	for (int i=size-1; i>0; i--) {
		int j = rand() % (i+1);
		int tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}
}

int weighted_random() {
	int r = rand() % 100;
	if (r < SINGLE_CELL_CHANCE) return 1;
	else if (r < DOUBLE_CELL_CHANCE + SINGLE_CELL_CHANCE) return 2;
	else if (r < TRIPLE_CELL_CHANCE + DOUBLE_CELL_CHANCE + SINGLE_CELL_CHANCE) return 3;
	else return 4;
}

// Helper function to latin square for kenken board, (make more randomized in the future)
void generate_latin_square(GameState* game) {
	if (game->size <= 0) return;

	// Generate base latin square
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			game->solution[i][j] = (j+i) % game->size+1;
		}
	}

	// Shuffle rows
	int row_order[MAX_SIZE];
	for (int i=0; i<game->size; i++) row_order[i] = i;
	shuffle(row_order, game->size);

	int tmp[MAX_SIZE][MAX_SIZE];
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			tmp[i][j] = game->solution[row_order[i]][j];
		}
	}
	
	// Shuffle cols
	int col_order[MAX_SIZE];
	for (int i=0; i<game->size; i++) col_order[i] = i;
	shuffle(col_order, game->size);
	
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			game->solution[i][j] = tmp[i][col_order[j]];
		}
	}

	// Symbol shuffle, ex: for 4x4, symbols = [3, 1, 4, 2], row = [1, 2, 3, 4], now row = [3, 1, 4, 2]
	int symbol_map[MAX_SIZE];
	for (int i=0; i<game->size; i++) symbol_map[i] = i+1;
	shuffle(symbol_map, game->size);

	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			game->solution[i][j] = symbol_map[game->solution[i][j]-1];
		}
	}

}

void assign_cage_op(GameState* game, Cage* cage) {
	// If cage size 1 than it is EQL op
	if (cage->size == 1) {
		cage->op = EQL;
		cage->target = game->solution[cage->coords[0][0]][cage->coords[0][1]];
		return;
	}

	// Get all values
	int vals[MAX_CELLS];
	for (int i=0; i<cage->size; i++) {
		vals[i] = game->solution[cage->coords[i][0]][cage->coords[i][1]];
	}

	// 2-Cell cages all operations are valid, larger only ADD and MUL allowed
	Operation rand_op;
	
	if (cage->size == 2) {
		int high = vals[0] > vals[1] ? vals[0] : vals[1];
		int low = vals[0] < vals[1] ? vals[0] : vals[1];

		Operation ops[] = {ADD, SUB, MUL, DIV, DIV, DIV}; // Div 3 times to weigh the odds
		rand_op = ops[rand() % 6]; 

		// If div doesn't work fallback to other operations
		if (rand_op == DIV && (low == 0 || high % low != 0)) {
			Operation fallback[] = {ADD, SUB, MUL};
			rand_op = fallback[rand() % 3];
		}
		
		cage->op = rand_op;
		
		switch (rand_op) {
			case ADD: cage->target = high+low; break;
			case SUB: cage->target = high-low; break;
			case MUL: cage->target = high*low; break;
			case DIV: cage->target = high/low; break;
			default: break;
		}
		return;
	}

	// Cages 3 and above
	Operation ops[] = {ADD, MUL};
	rand_op = ops[rand() % 2];

	cage->op = rand_op;
	cage->target = (rand_op == MUL) ? 1 : 0;
	
	for (int i=0; i<cage->size; i++) {
		if (rand_op == ADD) cage->target += vals[i];
		else cage->target *= vals[i];
	}
}

void generate_puzzle(GameState* game) {
	generate_latin_square(game);
	clear_grid(game);
	game->cage_count = 0;
	game->start_time = time(NULL);

	bool assigned[MAX_SIZE][MAX_SIZE] = {false};
	int assigned_num = 0; // Amount of assigned cells 

	// Loop until all cells assigned
	while (assigned_num < game->size*game->size) {
		int rand_x = rand() % game->size;
		int rand_y = rand() % game->size;
		if (assigned[rand_x][rand_y]) continue; // Skip assigned cells
		if (game->cage_count >= MAX_CAGES) break;
		
		// Create new cage
		Cage* cage = &game->cages[game->cage_count];
		cage->cage_id = game->cage_count+1;
		cage->size = 0;

		// Add cell
		cage->coords[cage->size][0] = rand_x;
		cage->coords[cage->size][1] = rand_y;
		cage->size++;
		assigned[rand_x][rand_y] = true;
		assigned_num++;

		int counter = 1;
		int cells = weighted_random();
		while (counter != cells && cage->size < MAX_CELLS) {
			int last_index = rand() % cage->size;
			int x = cage->coords[last_index][0];
			int y = cage->coords[last_index][1];

			// 4-directional neighbours
			int dirs[4][2] = {{1,0}, {-1, 0}, {0, 1}, {0, -1}};
			int order[4] = {0, 1, 2, 3};
			shuffle(order, 4);

			int found = 0;

			// Try random directions
			for (int i=0; i<4; i++) {
				int nx = x + dirs[order[i]][0];
				int ny = y + dirs[order[i]][1];

				// Bounds
				if (nx >= 0 && nx < game->size && ny >= 0 && ny < game->size && !assigned[nx][ny]) {
					if (cage->size > MAX_CELLS) break;
					cage->coords[cage->size][0] = nx;
					cage->coords[cage->size][1] = ny;
					cage->size++;

					assigned[nx][ny] = true;
					assigned_num++;

					counter++;
					found = 1;
					break;
				}

			}
			if (!found) break;
		}

		assign_cage_op(game, cage);
		game->cage_count++;
	}


// 	for (int i=0; i<game->size; i++) {
// 		for (int j=0; j<game->size; j++) {
// 			if (assigned[i][j]) continue;
// 
// 			// Create new cage at cell
// 			Cage* cage = &game->cages[game->cage_count];
// 			cage->cage_id = game->cage_count+1;
// 			cage->size = 0;
// 
// 			// Add cell
// 			cage->coords[cage->size][0] = i;
// 			cage->coords[cage->size][1] = j;
// 			cage->size++;
// 			assigned[i][j] = true;
// 
// 			// Grow cage into neighbour
// 			if (j+1 < game->size && !assigned[i][j+1]) {
// 				cage->coords[cage->size][0] = i;
// 				cage->coords[cage->size][1] = j+1;
// 				cage->size++;
// 				assigned[i][j+1] = true;
// 			}
// 
// 			assign_cage_op(game, cage);
// 
// 			game->cage_count++;
// 		}
// 	}

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
};



