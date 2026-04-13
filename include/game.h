#ifndef GAME_H
#define GAME_H
#include <time.h>
#include <stdbool.h>

#define SIZE 4
#define MAX_CAGES 16
#define MAX_CELLS 6 // Max cells per cage

#define CELL_W 6
#define CELL_H 3

typedef enum {
	ADD = '+',
	SUB = '-',
	MUL = '*',
	DIV = '/',
	EQL = '='
} Operation;

// Forward declerations
typedef struct Cage Cage;

typedef struct {
	int row;
	int col;
	Cage* cage;
} Cell;

struct Cage {
	int coords[MAX_CELLS][2];
	int size;
	Operation op;
	int target;
	int cage_id;
};


typedef struct {
	int grid[SIZE][SIZE]; // Current player input
	int solution[SIZE][SIZE]; // Solved grid
	Cell cells[SIZE][SIZE]; // Cell metadata
	Cage cages[MAX_CAGES];
	int cage_count;
	time_t start_time;
} GameState;

bool check_win(GameState* game);

#endif
