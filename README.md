# CPU-Matrix-Computation
Parallel algorithms to solve linear system, compute inverse, LU factorization, and determinants

This is a course project for EE382C Multicore Computing in UT Austin, conducted by Prof. Vijay K. Garg(garg@ece.utexas.edu).

This repo has 5 folders. The first four folders are the four matrix operations we solved. The las folder is the test case folder used for test and analyze the performance of our code.

## Assumptions for our code:
- Our implemenations assumes the input matrix does not have any zeroes on the diagonal. 
- Input files are have same format under the test case folder.


## How to run the code?

## Inverse: 
- parallel_inverse.c && seq_inverse.c
after the code is compiled. The exe file takes 2 input parameters. The first parameter is the name of the input file and the second parameter is the name of the output file. The input file must format like **largerNxN--.txt** files in the test case folder. That is, the first line of the txt file should be the N for an N * N matrix. Then, actual elements of the matrix (N * N) are sperated by space. 


## LinearSystem: 
- parallel_linearSys_pipeline.c:
after the code is compiled. The exe file takes 3 input parameters. The first parameter is the name of the input file; the second parameter is the name of the output file; the third argument represents the number of thread you want to use. **The thread must be divisible by N** For example, if N = 6, then the number of threads can take 1,2,3,6. Other inputs will be deemed illegal. The input file must format like **largerLinearSys--.txt** files in the test case folder. That is, the first line of the txt file should be the N for an N * (N+1) matrix. The last column of each row represents the result of linear equation. Then, actual elements of the matrix (N * N+1) are sperated by space. 

- parallel_linearSys_rc.c & seq_linear_sys.c
after the code is compiled. The exe file takes 2 input parameters. The first parameter is the name of the input file and the second parameter is the name of the output file. The input file must format like largerLinearSys--.txt files in the test case folder. That is, the first line of the txt file should be the N for an N * (N+1) matrix. The last column of each row represents the result of linear equation. Then, actual elements of the matrix (N * N+1) are sperated by space. 

## Determinants:

Put desired test cases into the Determinant folder and change the file name from "largerNxN50.txt" to your test case file
```
fp = fopen("largerNxN50.txt", "r");
```
or write the correct path of the test file to run. 

To change the number of thread used, please change the number in
```
omp_set_num_threads(2); // for parallelDeterminant1.c
```
or
```
determinantOfMatrix(matrix, n, 6); // for parallelDeterminant2.c
```
The test case are expected to firstly indicate the size (N) on the first row, and then provide a N x N matrix.

## LU Factorization

Put desired test cases into the Determinant folder and change the file name from "largerNxN50.txt" to your test case file
```
 fp = fopen("file.txt", "r");
```
or write the correct path of the test file to run. 

For parallelLUFactorization_rc.c, to change the number of thread used, either add
```
omp_set_num_threads(p); // p for number of threads
```
or set the number in the terminal and run.

For parallelLUFactorization-pipeline.c, currently can only take at most 2 threads. To change the number of thread, change the set in
```
luDecomposition(matrix, n, 2); // 2 here means setting 2 threads to run simultaneously
```
The test case are expected to firstly indicate the size (N) on the first row, and then provide a N x N matrix.
