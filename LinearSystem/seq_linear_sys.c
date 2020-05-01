#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printMatrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n+1; j++) {
            printf("%f, ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void linearMatrix(double** matrix, int n) {

    // swap row of matrix
    for (int i = n-1; i > 0; i--) {
        if (matrix[i-1][0] < matrix[i][0]) {
            double* row = matrix[i];
            matrix[i] = matrix[i-1];
            matrix[i-1] = row;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j != i) {
                double temp = matrix[j][i] / matrix[i][i];
                for (int k = 0; k < n+1; k++) {
                    matrix[j][k] -= matrix[i][k] * temp;
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        double temp = matrix[i][i];
        for (int j = 0; j < n+1; j++) {
            matrix[i][j] = matrix[i][j] / temp;
        }
    }

    printMatrix(matrix, n);
}

void main(void) {
    int i, j, n;
    FILE *fp;
    // read in file1
    fp = fopen("file.txt", "r");
    fscanf(fp, "%i", &(n));

    printf("n is %i \n", n);

    double** matrix = (double**)malloc(n*sizeof(double*));
    for (i = 0; i<n; i++){
        matrix[i] = (double*)malloc((n+1)*sizeof(double));
    }

    for (i = 0; i < n; ++i) {
        for (j = 0;j < n+1; ++j) {
            fscanf(fp, "%lf", &matrix[i][j]);
        }
    }
    fclose(fp);

    printMatrix(matrix,n);

    linearMatrix(matrix, n);
}