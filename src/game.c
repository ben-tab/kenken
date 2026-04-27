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

// Helper function to generate kenken board, (make more randomized in the future)
void generate_latin_square(GameState* game) {
	for (int i=0; i<game->size; i++) {
		for (int j=0; j<game->size; j++) {
			game->solution[i][j] = (j+i+game->size % game->size)+1;
		}
	}
}

void generate_puzzle(GameState* game) {
	
};


