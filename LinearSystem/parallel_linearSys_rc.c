#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
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

void printSolutions(double** matrix, int n, FILE *fp) {
    if (fp != NULL) {
        for (int i = 0; i < n; i++) {
            fprintf(fp, "%f, ",matrix[i][n]);
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    } else {
        for (int i = 0; i < n; i++) {
            printf("%f, ",matrix[i][n]);
            printf("\n");
        }
        printf("\n");
    }
}

void parallel_RowcCyclic(double** matrix, int n, int bs) {
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
                for (int j = k+1; j < n+1; j++) {
                    matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        matrix[i][n] = matrix[i][n]/matrix[i][i];
    }
}

void main(int argc, char *argv[]) {
    int i, j, n;
    char *input_file, *output_file;
    FILE *fp;
    clock_t start, end;
    
    input_file = argv[1];
    output_file = argv[2];

    if (input_file == NULL) fprintf( stderr, "Please enter input file" ); 
    if (output_file == NULL) fprintf( stderr, "Please enter output file" ); 

    fp = fopen(input_file, "r");
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

    start = clock();
    parallel_RowcCyclic(matrix, n, 16);
    end = clock();

    double runtime = (double) (end - start) / CLOCKS_PER_SEC;
    printf("parallel linear time (row cyclic) is %lf: \r\n", runtime);

    fp = fopen(output_file, "w");
    printSolutions(matrix,n,fp);
    fclose(fp);

    free(matrix);
}