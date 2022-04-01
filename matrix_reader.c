#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ARRAY_LENGTH 100
#define MAX_FLOAT_VALUE 10




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



int main(int argc, char *argv[]){
	int number_of_nz = 0;

	if (argc != 4){
		printf("Error: you need to provide R (rows) and C (columns) of the matrix to read\n");
		exit(0);
	}
	
	FILE* p;
	p = fopen("matrix.txt", "r");
	if (p == NULL) printf("Error in reading the file\n");
	

	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	float num_read;

	/*
	//CLASSIC ARRAY DEFINITION
	int row[ARRAY_LENGTH];
	int indices[ARRAY_LENGTH];
	float data[ARRAY_LENGTH];
	*/

	//MALLOC ARRAY DEFINITION
	int *row;
	int *indices;
	int *ptr;
	float *data;

	//CSR AND COO
	
	indices = (int*) malloc(ARRAY_LENGTH * sizeof(int));
	data = (float*) malloc(ARRAY_LENGTH * sizeof(float));

	//COO
	row = (int*) malloc(ARRAY_LENGTH * sizeof(int));

	//CSR
	ptr = (int*) malloc(ARRAY_LENGTH * sizeof(int));
	
	if (row == NULL || indices == NULL || data == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
  	}
  	

	int data_iterator = 0;
	int row_iterator = 0;
	int indices_iterator = 0;
	int ptr_iterator = 0;
	int i,j;

	ptr[0] = 0;
	ptr_iterator = 1;


	//READING THE MATRIX FOR COO AND CSR
	for (i = 0; i < rows; i++){
		for (j = 0; j < cols; j++){
			
			fscanf(p,"%f",&num_read);


			if (num_read != 0){
				data[data_iterator] = num_read;
				
				row[row_iterator] = i;
				indices[indices_iterator] = j;

				data_iterator++;
				row_iterator++;
				indices_iterator++;

				number_of_nz++;
			}
		}
		//adding the current number_of_nz to the ptr array
		ptr[ptr_iterator] = number_of_nz;
		ptr_iterator++;
	}


/****************************************************************************************/

	//READING THE MATRIX FOR ELL
	int max_nz_per_row = 0;
	int nz_per_row = 0;
	float *data_ELL;
	float *data_EL2;
	int *indices_ELL;
	int *indices_EL2;


	

	if (strcmp(argv[3],"ELL")==0){
		
		rewind(p);

		//checking what is the max number of non-zero per row in the matrix to allocate an array for ELL format
		for (i = 0; i < rows; i++){
			for (j = 0; j < cols; j++){
				fscanf(p,"%f",&num_read);
				if (num_read != 0){
					nz_per_row++;
				}
			}
			if (nz_per_row > max_nz_per_row){
				max_nz_per_row = nz_per_row;
			}
			nz_per_row = 0;
		}
		
		rewind(p);

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
				fscanf(p,"%f",&num_read);
				if (num_read != 0){
					data_ELL[max_nz_per_row*i + data_ELL_iterator] = num_read;
					indices_ELL[max_nz_per_row*i + indices_ELL_iterator] = j;
					data_ELL_iterator++;
					indices_ELL_iterator++;
				}
			}
			data_ELL_iterator = 0;
			indices_ELL_iterator = 0;
		}

		//print_ELL(data_EL2, indices_EL2, rows, max_nz_per_row);
		

	}


/****************************************************************************************/


	float *x = generate_x(cols);

	//print_info(rows, cols, number_of_nz);

	float *y = (float*) malloc(cols * sizeof(float));
	

	if(strcmp(argv[3],"COO")==0){
		
		spmv_coo(row, indices, data, number_of_nz, x, y);
	}

	else if (strcmp(argv[3],"CSR")==0){
		spmv_csr(ptr, indices, data, number_of_nz, x, y);
	}

	else if (strcmp(argv[3],"ELL")==0){
		switch_float_array(data_ELL, data_EL2, rows, max_nz_per_row);
		switch_int_array(indices_ELL, indices_EL2, rows, max_nz_per_row);
		//print_ELL(data_EL2, indices_EL2, rows, max_nz_per_row);
		spmv_ell(indices_EL2, data_EL2, max_nz_per_row, rows, x, y);
	}


	//print_debug(row, indices, data, number_of_nz, x, y, cols);

	

	print_y(y, cols);

	return 0;
}




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

void print_info(rows, cols, number_of_nz){
	float rows_float = rows;
	float cols_float = cols;
	float number_of_nz_float = number_of_nz;
	printf("Number of NZ: %d\n", number_of_nz);
	printf("Density: %f\n", number_of_nz_float/(rows_float*cols_float));
}

void print_y(float *y, int length){
	printf("\nARRAY y: \n");
	for (int i = 0; i < length; i++){
		printf("y[%d] = %f\n", i, y[i]);
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








/*
void print_debug(int *row, int *indices, float *data, int number_of_nz, float *x, float *y, int cols, int *ptr){
	printf("\n");
	for (int i = 0; i < number_of_nz; i++){
		printf("row[%d] = %d\n", i, row[i]);
	}
	printf("\n");
	for (int i = 0; i < number_of_nz; i++){
		printf("indices[%d] = %d\n", i, indices[i]);
	}
	printf("\n");
	for (int i = 0; i < number_of_nz; i++){
		printf("data[%d] = %f\n", i, data[i]);
	}
	printf("\n");
	for (int i = 0; i < number_of_nz; i++){
		printf("ptr[%d] = %d\n", i, ptr[i]);
	}
	printf("\n");
	for (int i = 0; i < cols; i++){
		printf("x[%d] = %f\n", i, x[i]);
	}
	printf("\n");
	for (int i = 0; i < cols; i++){
		printf("y[%d] = %f\n", i, y[i]);
	}
	printf("\n");
}
*/
