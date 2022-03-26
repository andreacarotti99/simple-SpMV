# simple-SpMV

simple-SpMV are some scripts in C made for a naive implementation of Sparse Matrix Vector Multiplication (SpMV):

y = Ax

# Run simple-SpMV

### Generating a MxN sparse matrix

First we need to generate a matrix in the directory passing as arguments
- M: Number of rows of the generated matrix
- N: Number of columns of the generated matrix
- D: Density of the matrix in decimal format

```bash
gcc sparse_matrix_generator.c -o sparse_matrix_generator
./sparse_matrix_generator 10 10 0.67
```

will generate a 100x100 matrix in a matrix.txt file with approximately 67 non-zero elements ou of 100


### Running SpMV

```bash
gcc matrix_reader.c -o matrix_reader
./matrix_reader 10 10
```

will run SpMV with the generated matrix
