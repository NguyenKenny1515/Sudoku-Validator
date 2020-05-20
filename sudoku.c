#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#define GRID_SIZE 9
#define NUM_THREADS 11

typedef struct{
	int row;
	int column;
	int puzzle[GRID_SIZE][GRID_SIZE];
	int result;
} parameters;

void sudoku(int puzzle[GRID_SIZE][GRID_SIZE]);
void *columns(parameters *data);
void *rows(parameters *data);
void *grid(parameters *data);

int main(void) {
	int puzzle1[GRID_SIZE][GRID_SIZE] = {{7,3,9,8,4,6,1,2,5}, {4,1,2,9,7,5,8,6,3}, {8,6,5,2,3,1,9,4,7},
										 {5,4,7,6,0,3,2,1,8}, {3,9,1,7,8,2,4,5,6}, {2,8,6,5,1,4,3,7,9},
	                                     {9,2,8,4,6,7,5,3,1}, {1,7,4,3,5,9,1,8,2}, {6,5,3,1,2,8,7,9,4}};
	int puzzle2[GRID_SIZE][GRID_SIZE] = {{4,1,9,3,7,6,5,8,2}, {3,6,7,8,5,2,1,4,9}, {2,8,5,1,9,4,3,7,6},
										 {6,4,1,9,3,7,8,2,5}, {9,3,2,4,8,5,6,1,7}, {5,7,8,2,6,1,4,9,3},
	                                     {8,5,6,7,4,9,2,3,1}, {1,9,4,5,2,3,7,6,8}, {7,2,3,6,1,8,9,5,4}};
	sudoku(puzzle1);
	sudoku(puzzle2);
	return 0;
}

void sudoku(int puzzle[GRID_SIZE][GRID_SIZE]) {
	// Print out the grid
	printf("\n");
	for (int row = 0; row < GRID_SIZE; row++) {
		for (int col = 0; col < GRID_SIZE; col++) {
			printf("%d ", puzzle[row][col]);
		}
		printf("\n");
	}

	pthread_t workers[NUM_THREADS];

	// Creates array of parameters for each thread and copies over puzzle for each thread
    parameters *data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        data[i] = (parameters *) malloc(sizeof(parameters));
        memcpy(data[i]->puzzle, (int *)puzzle, 81 * sizeof(int));
        data[i]->result = 1;
    }

    // Create 2 workers to check columns and rows. Set their locations in parameter
    for (int i = 0; i < 2; i++) {
    	data[i]->row = 0;
    	data[i]->column = 0;
    	if (i == 0)
			pthread_create(&workers[i], NULL, (void *)columns, data[i]);
    	else
			pthread_create(&workers[i], NULL, (void *)rows, data[i]);
    }

    // Create 9 workers to check the 3x3 grids. Set their locations in parameter
	int count = 2;
	for (int i = 0; i < GRID_SIZE; i += 3) {
		for (int j = 0; j < GRID_SIZE; j+= 3) {
			data[count]->row = i;
			data[count]->column = j;
			pthread_create(&workers[count++], NULL, (void *)grid, data[count]);
		}
	}

	// Main thread waits on workers
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(workers[i], NULL);
	}

    // Check if any number in results is 0. If so, puzzle is invalid
	bool valid = true;
	for (int i = 0; i < NUM_THREADS; i++) {
		if (data[i]->result == 0) {
			valid = false;
			break;
		}
	}
	if (valid)
		printf("Puzzle is valid\n");
	else
		printf("Puzzle is not valid\n");

	// After all worker threads terminate, free up their struct data
    for (int i = 0; i < NUM_THREADS; i++)
        free(data[i]);
}

void *columns(parameters *data) {
	for (int row = data->row; row < GRID_SIZE; row++) {
		int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Keeps track of which numbers exist in column

		for (int col = data->column; col < GRID_SIZE; col++) {
			int num = data->puzzle[col][row];

			if (num < 1 || num > 9) { // If value not between 1 and 9
				data->result = 0;
				printf("column[%d], value %d not between 1 and 9\n", row, num);
			}

			if (num >= 1 && num <= 9) // Increment the count value index
				count[num]++;
		}

		for (int i = 1; i <= 9; i++) {
			if (count[i] == 0) { // If missing value
				data->result = 0;
				printf("column[%d], missing value %d\n", row, i);
			}
			else if (count[i] >  1) { // If duplicate value
				data->result = 0;
				printf("column[%d], duplicate value %d\n", row, i);
			}
		}
	}

	pthread_exit(0);
}

void *rows(parameters *data) {
	for (int row = data->row; row < GRID_SIZE; row++) {
		int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Keeps track of which numbers exist in row

		for (int col = data->column; col < GRID_SIZE; col++) {
			int num = data->puzzle[row][col];

			if (num < 1 || num > 9) { // If value not between 1 and 9
				data->result = 0;
				printf("row[%d], value %d not between 1 and 9\n", row, num);
			}

			if (num >= 1 && num <= 9) // Increment the count value index
				count[num]++;
		}

		for (int i = 1; i <= 9; i++) {
			if (count[i] == 0) { // If missing value
				data->result = 0;
				printf("row[%d], missing value %d\n", row, i);
			}
			else if (count[i] >  1) { // If duplicate value
				data->result = 0;
				printf("row[%d], duplicate value %d\n", row, i);
			}
		}
	}
	pthread_exit(0);
}

void *grid(parameters *data) {
	int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Keeps track of which numbers exist in 3x3 grid

	for (int row = data->row; row < data->row + 3; row++) {
		for (int col = data->column; col < data->column + 3; col++) {
			int num = data->puzzle[row][col];

			if (num < 1 || num > 9) { // If value not between 1 and 9
				data->result = 0;
				printf("puzzle[%d][%d] = %d, not between 1 and 9\n", row, col, num);
			}

			if (num >= 1 && num <= 9)  { // Increment the count value index
				count[num]++;
				if (count[num] > 1) { // If duplicate value
					data->result = 0;
					printf("puzzle[%d][%d] = %d, duplicate value\n", row, col, num);
				}
			}
		}
	}

	for (int i = 1; i <= 9; i++) {
		if (count[i] == 0) { // If missing value
			data->result = 0;
			printf("puzzle[%d][%d], missing value %d\n", data->row, data->column, i);
		}
	}
	pthread_exit(0);
}
