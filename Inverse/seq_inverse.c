#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printMatrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 2 * n; j++) {
            printf("%f, ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void inverseMatrix(double** matrix, int n) {
    // append identity matrix
    for (int i = 0; i < n; i++) {
        matrix[i][i+n] = 1;
    }

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
                for (int k = 0; k < 2 * n; k++) {
                    matrix[j][k] -= matrix[i][k] * temp;
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        double temp = matrix[i][i];
        for (int j = 0; j < 2 * n; j++) {
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
        matrix[i] = (double*)malloc(2*n*sizeof(double));
    }

    for (i = 0; i < n; ++i) {
        for (j = 0;j < n; ++j) {
            fscanf(fp, "%lf", &matrix[i][j]);
        }
        for (;j < 2*n; j++) {
            matrix[i][j] = 0;
        }
    }
    fclose(fp);

    printMatrix(matrix,n);

    inverseMatrix(matrix, n);
}