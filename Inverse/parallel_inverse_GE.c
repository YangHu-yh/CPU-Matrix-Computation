#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

void printMatrix(double** matrix, int n, FILE *fp) {
    if (fp != NULL) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n+1; j++) {
                fprintf(fp, "%f, ",matrix[i][j]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n+1; j++) {
                printf("%f, ",matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void printInverseMatrix(double** matrix, int n, FILE *fp) {
    if (fp != NULL) {
        for (int i = 0; i < n; i++) {
            for (int j = n; j < 2 * n; j++) {
                fprintf(fp, "%f, ",matrix[i][j]);
            }
            fprintf(fp,"\n");
        }
        fprintf(fp,"\n");
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = n; j < 2 * n; j++) {
                printf("%f, ",matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void parallel_inverse(double** matrix, int n, int bs) {
    for (int k = 0; k < n; k++) {
        
        #pragma omp parallel for schedule(static, bs)
        for (int i = 0; i < n; i++) {
            if (i != k) {
                matrix[i][k] = matrix[i][k] / matrix[k][k];
            }
        }

        #pragma omp parallel for schedule(static, bs)
        for (int i = 0; i < n; i++) {
            if (i != k) {
                for (int j = k+1; j < 2*n; j++) {
                    matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                }
            }
        }
    }

    #pragma omp parallel for schedule(static, bs)
    for (int i = 0; i < n; i++) {
        for (int j = n; j < 2*n; j++) {
            matrix[i][j] = matrix[i][j] / matrix[i][i];
        }
    }
}

void main(void) {
    int i, j, n;
    FILE *fp;
    clock_t start, end;
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
            if (j - n == i) {
                matrix[i][j] = 1;
            } else {
                matrix[i][j] = 0;
            }

        }
    }

    start = clock();
    parallel_inverse(matrix, n, 2);
    end = clock();

    double runtime = (double) (end - start) / CLOCKS_PER_SEC;
    printf("parallel inverse time is %lf: \r\n", runtime);

    printInverseMatrix(matrix,n, NULL);
}