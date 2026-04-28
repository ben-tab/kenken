#ifndef DEFS_H
#define DEFS_H

#include <time.h>
#include <stdbool.h>

#define MAX_SIZE 9 // Max grid size 9x9
#define MIN_SIZE 3 // Mind grid size 3x3
#define MAX_CAGES (MAX_SIZE*MAX_SIZE) // Max amount of cages
#define MAX_CELLS 4 // Max cells per cage

// Enum of all operations
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
	int coords[MAX_CELLS][2]; // Coordinate of cell in cage (y, x) ex: ({2, 2}, {2, 3}, {1, 3})
	int size;
	Operation op;
	int target;
	int cage_id;
};

typedef struct {
	int grid[MAX_SIZE][MAX_SIZE]; // Current player input
	int solution[MAX_SIZE][MAX_SIZE]; // Solved grid
	Cell cells[MAX_SIZE][MAX_SIZE]; // Cell metadata
	Cage cages[MAX_CAGES];
	int cage_count;
	time_t start_time;
	int size;
} GameState;

#endif
