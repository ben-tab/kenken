#include "../include/game.h"

bool check_win(GameState* game) {
	for (int i=0; i<SIZE; i++) {
		for (int j=0; j<SIZE; j++) {
			if (game->grid[i][j] != game->solution[i][j]) {
				return false;
			}
		}
	}
	return true;
}
