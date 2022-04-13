#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


#define ARRAY_LENGTH 100
#define MAX_FLOAT_VALUE 10
#define ROWS 4
#define COLS 4
#define DENSITY 0.2
#define CODIFICA "COO"


/******************************************************************* DECLARING FUNCTIONS *********************************************************************/

void spmv_coo();
void spmv_csr();
void spmv_ell();

float *generate_x();
void print_info();
void print_y();
void print_debug();
void print_ELL();
void switch_float_array();
void switch_int_array();
void print_matrix();

/******************************************************************** MAIN *************************************************************************************/

int main(int argc, char *argv[]){
	printf("\nInitializing all variables...\n");

	int number_of_nz = 0;
	float mat[ROWS][COLS];
	int rows = ROWS;
	int cols = COLS;
	float num_read;
	float density = DENSITY;
	int random_to_generate = density*rows*cols;
	int random_row;
	int random_cols;
	int *row;
	int *indices;
	int *ptr;
	float *data;
	int data_iterator = 0;
	int row_iterator = 0;
	int indices_iterator = 0;
	int ptr_iterator = 0;
	int i,j;
	int max_nz_per_row = 0;
	int nz_per_row = 0;
	float *data_ELL;
	float *data_EL2;
	int *indices_ELL;
	int *indices_EL2;

	printf("\nInitializing matrix all zeros...\n");


	for (int i = 0; i < ROWS; i++){
		for (int j = 0; j < COLS; j++){
			mat[i][j] = 0;
		}
	}

	printf("\nPopulating matrix with random values...\n");
	for (int i=0;i<random_to_generate;i++){
		random_row = rand() % rows;
		random_cols = rand() % cols;
		mat[random_row][random_cols] = (float)rand()/(float)(RAND_MAX/MAX_FLOAT_VALUE);
	}

	print_matrix(mat, rows, cols);


	printf("\nAllocating data structures for spmv...\n");
	indices = (int*) malloc(ARRAY_LENGTH * sizeof(int));
	data = (float*) malloc(ARRAY_LENGTH * sizeof(float));
	row = (int*) malloc(ARRAY_LENGTH * sizeof(int));
	ptr = (int*) malloc(ARRAY_LENGTH * sizeof(int));
	

	if (row == NULL || indices == NULL || data == NULL) {
    printf("Memory not allocated.\n");
    exit(0);
  	}

	ptr[0] = 0;
	ptr_iterator = 1;

	printf("\nStarting reading the matrix to populate data structures...\n");
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			if (mat[i][j] != 0){
				data[data_iterator] = mat[i][j];
				row[row_iterator] = i;
				indices[indices_iterator] = j;
				data_iterator++;
				row_iterator++;
				indices_iterator++;
				number_of_nz++;
			}
		}
		ptr[ptr_iterator] = number_of_nz;
		ptr_iterator++;
	}


/**************************************************************************************************************************************************/

	if (strcmp(CODIFICA,"ELL")==0){
		
		for (i = 0; i < rows; i++){
			for (j = 0; j < cols; j++){
				//fscanf(p,"%f",&num_read);

				if (mat[i][j] != 0){
					nz_per_row++;
				}
			}
			if (nz_per_row > max_nz_per_row){
				max_nz_per_row = nz_per_row;
			}
			nz_per_row = 0;
		}


		data_ELL = (float*) malloc(rows*max_nz_per_row * sizeof(float));
		data_EL2 = (float*) malloc(rows*max_nz_per_row * sizeof(float));
		indices_ELL = (int*) malloc(rows*max_nz_per_row * sizeof(int));
		indices_EL2 = (int*) malloc(rows*max_nz_per_row * sizeof(int));



		if (data_ELL == NULL || indices_ELL == NULL) {
      printf("Memory not allocated.\n");
      exit(0);
  	}

  	int data_ELL_iterator = 0;
  	int indices_ELL_iterator = 0;

 		for (i = 0; i < rows; i++){
			for (j = 0; j < cols; j++){
				if (mat[i][j] != 0){
					data_ELL[max_nz_per_row*i + data_ELL_iterator] = mat[i][j];
					indices_ELL[max_nz_per_row*i + indices_ELL_iterator] = j;
					data_ELL_iterator++;
					indices_ELL_iterator++;
				}
			}
			data_ELL_iterator = 0;
			indices_ELL_iterator = 0;
		}
	}


/**************************************************************************************************************************************************/


	float *x = generate_x(cols);
	float *y = (float*) malloc(cols * sizeof(float));


	if(strcmp(CODIFICA,"COO")==0){
		printf("\nStarting multiplication with COO format...\n");
		spmv_coo(row, indices, data, number_of_nz, x, y);
	}
	else if (strcmp(CODIFICA,"CSR")==0){
		printf("\nStarting multiplication with CSR format...\n");
		spmv_csr(ptr, indices, data, number_of_nz, x, y);
	}
	else if (strcmp(CODIFICA,"ELL")==0){
		switch_float_array(data_ELL, data_EL2, rows, max_nz_per_row);
		switch_int_array(indices_ELL, indices_EL2, rows, max_nz_per_row);
		//print_ELL(data_EL2, indices_EL2, rows, max_nz_per_row);
		printf("\nStarting multiplication with ELL format...\n");
		spmv_ell(indices_EL2, data_EL2, max_nz_per_row, rows, x, y);
	}
	print_y(y, cols);
	printf("Program successfully terminated.\n");
	return 0;
}

/****************************************************************** FUNCTIONS ********************************************************************************/

void spmv_ell(int *indices, float *data, int max_nz_per_row, int rows, float *x, float *y) {
  int i, j;
  for(j = 0; j <  max_nz_per_row; j++){
    for(i = 0; i < rows; i++){
    	//printf("%d\n", max_nz_per_row);
      y[i] += data[j * rows + i] * x[indices[j * rows + i]];
      //printf("y[%d] = data[%d * %d + %d] * x[indices[%d * %d + %d] = %f\n",i,j,max_nz_per_row,i,j,max_nz_per_row,i, y[i]);

    }
  }
}

void spmv_coo(int *rowind, int *colind, float *val, int number_of_nz, float *x, float *y) {
  int i;
  for(i = 0; i < number_of_nz ; i++){
    y[rowind[i]] += val[i] * x[colind[i]];
    //printf("y[%d] = %f\n", i, y[i]);
  }
}


void spmv_csr(int *row_ptr, int *colind, float *val, int number_of_nz, float *x, float *y)
{
  int i, j;
  float temp;
  for(i = 0; i < number_of_nz ; i++)
  {
    temp = y[i];
    for(j = row_ptr[i]; j < row_ptr[i+1]; j++){
      temp += val[j] * x[colind[j]];
    }
    y[i] = temp;
  }
}

/****************************************************************** SUPPORT FUNCTIONS ********************************************************************************/


float *generate_x(int C){
	printf("\nGENERATING RANDOM DENSE ARRAY x:\n");
	srand(time(NULL));
	float r;
	int i;

	float *array = (float*) malloc(C * sizeof(float));
	for (i=0; i<C; i++){
		//array[i] = (float)rand()/(float)(RAND_MAX/MAX_FLOAT_VALUE);
		array[i] = 2;
		printf("generated... x[%d] = %f\n",i, array[i]);
	}
	return array;
}


void print_y(float *y, int length){
	float y_float;
	float small_float;
	float y_cut;
	int small_int;
	int y_int;


	printf("\nARRAY y: \n");
	printf("(The array is printed with int values instead of float...)\n");
	for (int i = 0; i < length; i++){

		y_int = (int) y[i];
		y_float = y[i];
		y_cut = (float) y_int;
		small_float = y_float - y_int;
		small_float = small_float * 100;
		small_int = (int) small_float;


		printf("y[%d] = %d.%d\n", i, y_int, small_int);
	}
}

void print_ELL(float *d, int *ind, int R, int max_nz_per_row){

	printf("\nELL FORMAT DATA STRICTURES:\ndata = [");
	for (int i = 0; i < max_nz_per_row * R; i++){
		printf("%f ", d[i]);
	}
	printf("]\nindices = [");
	for (int i = 0; i < max_nz_per_row * R; i++){
		printf("%d ", ind[i]);
	}
	printf("]\n");
}




void switch_float_array (float *array1, float *array2, int rows, int cols){
	//inizializzo array2
	for (int i = 0; i < rows*cols; i++){
		array2[i] = 0;
	}

	int switch_iterator = 0;

	for (int k = 0; k < cols; k++){
		for (int i = k; i < rows*cols; i = i+cols){
			array2[switch_iterator] = array1[i];
			switch_iterator++;
		}
	}

	//DEBUG
	/*
	printf("\nI've switched two float arrays...\n");
	printf("array2 = [ ");
	for (int i = 0; i < rows*cols; i++){
		printf("%f ", array2[i]);
	}
	printf("]\n");*/
}


void switch_int_array (int *array1, int *array2, int rows, int cols){
	//inizializzo array2
	for (int i = 0; i < rows*cols; i++){
		array2[i] = 0;
	}

	int switch_iterator = 0;

	for (int k = 0; k < cols; k++){
		for (int i = k; i < rows*cols; i = i+cols){
			array2[switch_iterator] = array1[i];
			switch_iterator++;
		}
	}

	//DEBUG
	/*
	printf("\nI've switched two int arrays...\n");
	printf("array2 = [ ");
	for (int i = 0; i < rows*cols; i++){
		printf("%d ", array2[i]);
	}
	printf("]\n");
	*/
}



void print_matrix(float *mat, int R, int C){

	float mat_float;
	float small_float;
	float mat_cut;
	int small_int;
	int mat_int;


	printf("\nPrinting the matrix...\n");
	for (int i = 0; i < R; i++){
		for(int j = 0; j < C; j++){

			mat_int = (int) mat[i*C+j];
			mat_float = mat[i*C+j];
			mat_cut = (float) mat_int;
			small_float = mat_float - mat_int;
			small_float = small_float * 100;
			small_int = (int) small_float;

			printf("%d.%d ", mat_int, small_int);
		}
		printf("\n");
	}

}
