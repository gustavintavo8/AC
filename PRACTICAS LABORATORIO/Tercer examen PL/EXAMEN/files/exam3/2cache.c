#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// This program tansposes a randomly initialized matrix
#define N 2048
#define M 8092

int matrix[N][M];
int transpose[M][N];

int main(void) {
	uint16_t i, j;

	srand(time(NULL));

	// Initialization
	for (j = 0; j < M; j++) {
		for (i = 0; i < N; i++) {
			matrix[i][j] = rand();
		}
	}

	// Transpose
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			transpose[j][i] = matrix[i][j];
		}
	}
}
