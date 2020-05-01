// C Program to decompose a matrix into 
// lower and upper traingular matrix 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <omp.h>

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
    printf("Setting lower and upper...");
    double** lower = (double**)malloc(n*sizeof(double*));
    double** upper = (double**)malloc(n*sizeof(double*));
    for (int i = 0; i<n; i++){
        lower[i] = (double*)malloc((n)*sizeof(double));
        upper[i] = (double*)malloc((n)*sizeof(double));
    }
    for (int i = 0; i < n; i++){
        lower[i][i] = 1.;
        for (int j = 0; j < n ; j++){
            upper[i][j] = matrix[i][j];
        }
    }
  
    // Decomposing matrix into Upper and Lower 
    // triangular matrix 
    double t = omp_get_wtime();
    for (int k = 0; k < n-1; k++) { 
        // printf("start division...\n");
        #pragma omp parallel for schedule(dynamic)
        {
            for (int i = k+1; i < n; i++) { 
                if(i != k)
                    lower[i][k] = upper[i][k]/upper[k][k];
                    // printf("%f = %f/%f\n",lower[i][k],upper[i][k],upper[k][k]);
            } 
        }
  
        // Upper Triangular 
        // printf("start elimination...\n");
        #pragma omp parallel for schedule(dynamic)
        {
            for (int j = k+1; j < n; j++) { 
                for(int i = k+1; i < n; i++){
                    upper[i][j] = upper[i][j] - lower[i][k]*upper[k][j];
                    // printf("%f -= %f * %f\n",upper[i][j],lower[i][j],upper[k][j]);
                }
            } 
        }
    }
    for (int j = 0; j < n; j++) { 
        for(int i = 0; i < n; i++){
            if(i > j){
                upper[i][j] = 0.;
            }
        }
    } 
    printf("excution time is %0.5lf seconds\n", (omp_get_wtime()-t));
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
    fp = fopen("file_six_six.txt", "r");
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
    printf("Decomposing...\n");
    luDecomposition(matrix, n); 
    return 0; 
} 
