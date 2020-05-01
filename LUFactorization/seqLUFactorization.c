// CPP Program to decompose a matrix into 
// lower and upper traingular matrix 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

// print matrix	
void printNxNMatrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f, ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
  
void luDecomposition(double** matrix, int n) 
{ 
    // int lower[n][n], upper[n][n]; 
    // memset(lower, 0, sizeof(lower)); 
    // memset(upper, 0, sizeof(upper)); 
    double** lower = (double**)malloc(n*sizeof(double*));
    double** upper = (double**)malloc(n*sizeof(double*));
    for (int i = 0; i<n; i++){
        lower[i] = (double*)malloc((n)*sizeof(double));
        upper[i] = (double*)malloc((n)*sizeof(double));
    }
  
    // Decomposing matrix into Upper and Lower 
    // triangular matrix 
    for (int i = 0; i < n; i++) { 
  
        // Upper Triangular 
        for (int k = i; k < n; k++) { 
  
            // Summation of L(i, j) * U(j, k) 
            double sum = 0; 
            for (int j = 0; j < i; j++) 
                sum += (lower[i][j] * upper[j][k]); 
  
            // Evaluating U(i, k) 
            upper[i][k] = matrix[i][k] - sum; 
        } 
  
        // Lower Triangular 
        for (int k = i; k < n; k++) { 
            if (i == k) 
                lower[i][i] = 1; // Diagonal as 1 
            else { 
  
                // Summation of L(k, j) * U(j, i) 
                double sum = 0; 
                for (int j = 0; j < i; j++) 
                    sum += (lower[k][j] * upper[j][i]); 
  
                // Evaluating L(k, i) 
                lower[k][i] = (matrix[k][i] - sum) / upper[i][i]; 
            } 
        } 
    } 

    // displaying lower matrix
    int i, j;
    printf("Lower Matrix:\n");
	printNxNMatrix(lower, n);
    // displaying upper matrix
    printf("Upper matrix:\n");
	printNxNMatrix(upper, n);
} 
  
// Driver code 
int main() 
{ 
    int i, j, n;
    FILE *fp;
    // read in file1
    fp = fopen("file.txt", "r");
    fscanf(fp, "%i", &(n));

    printf("n is %i \n", n);

    double** matrix = (double**)malloc(n*sizeof(double*));
    for (i = 0; i<n; i++){
        matrix[i] = (double*)malloc((n)*sizeof(double));
    }

    for (i = 0; i < n; ++i) {
        for (j = 0;j < n; ++j) {
            fscanf(fp, "%lf", &matrix[i][j]);
        }
    }
    fclose(fp);
    printf("Input matrix:\n");
    printNxNMatrix(matrix, n);
    // int mat0[][MAX] = { {3, 1},
    //                     {4, 2}};

    // int mat1[][MAX] = { { 2, -1, -2 }, 
    //                    { -4, 6, 3 }, 
    //                    { -4, -2, 8 } }; 

    // int mat[N][N] = { { 3, -7, -2,  2},
    //                    {-3,  5,  1,  0},
    //                    { 6, -4,  0, -5},
    //                    {-9,  5, -5, 12}};
  
    luDecomposition(matrix, n); 
    return 0; 
} 