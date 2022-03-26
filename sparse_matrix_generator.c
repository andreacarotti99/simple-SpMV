#include <stdio.h>
#include <stdlib.h>  // for strtol
#include <time.h>
#include <math.h>
#define MAX_FLOAT_VALUE 10000


/*
you have to pass as arguments:
- length of the matrix: rows and cols
- density of the matrix: density = ((number_of_nz) / N*N) e.g. 0.67
*/
int main(int argc, char *argv[]){
	
	if(argc != 4){
		printf("Error: Wrong number of arguments\n");
		printf("Provide the number of rows R, the number of columns C and the density of the matrix D\n");
		return 0;
	}


	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	float density = atof(argv[3]);
	int random_to_generate = (density*rows*cols);
	int random_row;
	int random_cols;
	float x;

	if (density > 1 || density < 0){
		printf("Error: Density must be between 0 and 1\n");
		return 0;
	}

	float **mat = (float **)malloc(rows * sizeof(float*));
	for(int i = 0; i < rows; i++) {
		mat[i] = (float *) malloc(cols * sizeof(float));
	}

	srand(time(NULL));

	for (int i =0; i < rows; i++){
		for(int j=0; j < cols; j++){
			mat[i][j] = 0;
		}
	}

	for (int i=0;i<random_to_generate;i++){
		random_row = rand() % rows;
		random_cols = rand() % cols;
		x = (float)rand()/(float)(RAND_MAX/MAX_FLOAT_VALUE);
		mat[random_row][random_cols] = x;
	}

	FILE *fp;
	fp  = fopen ("matrix.txt", "w");
	for (int i =0; i < rows; i++){
		int j;
		for(j=0; j < cols - 1; j++){
			fprintf(fp, "%f ", mat[i][j]);
		}
		if (j == cols - 1){
			fprintf(fp, "%f", mat[i][j]);
		}

		fprintf(fp, "\n");
		
	}

	printf("File created...\n");


}