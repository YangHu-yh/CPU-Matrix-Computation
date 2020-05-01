#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <omp.h>

void printNxNMatrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // printf("%f, ",matrix[i][j]);
        }
        // printf("\n");
    }
    // printf("\n");
}

void printNxNSubMatrix(double** sub, int m) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            // printf("%f, ",sub[i][j]);
        }
        // printf("\n");
    }
    // printf("\n");
}

double** submatrix(double** matrix, double** sub, int n, int i) {
    // printf("matrix submitted to calculate submatrix:\n");
    printNxNSubMatrix(matrix, n);
    for (int y = 1; y < n; y++) {
        int sub_index = 0;
        for (int x = 0; x < n; x++) {
            if (x != i) {
                sub[y-1][sub_index] = matrix[y][x];
                // printf("sub[%d][%d] = matrix[%d][%d] = %f\n", y-1, sub_index, y,x, sub[y-1][sub_index]);
                sub_index += 1;
            }
        }
    }
    return sub;
}

double det2x2(double** matrix, int n) {
    double det;
    if (n != 2) {
        exit(0);
    }
    det = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
    return det;
}

double det2_seq(double** sub, int m) {
    // printf("input size int m = %d.\n",m);
    // printf("sub in det2_seq:\n");
    printNxNSubMatrix(sub, m);
    double det = 0;
    if (m == 2) det = det2x2(sub, m);
    else {
        double*** subs = (double***)malloc((m)*sizeof(double**));
        for(int m1 = 0; m1 < m; m1++){
            subs[m1] = (double**)malloc((m-1)*sizeof(double*));
            for(int m2 = 0; m2 < m-1; m2++)
                subs[m1][m2] = (double*)malloc((m-1)*sizeof(double));
        }

        for (int i = 0; i < m; i++) {
            double x = sub[0][i];
            double sign = (double) pow(-1.0, 2.0 + i);
            subs[i] = submatrix(sub, subs[i], m, i);
            printNxNSubMatrix(subs[i], m-1);
            det += sign * x * det2_seq(subs[i], m-1);
        }
    }
    // printf("have determinate %f.\n", det);
    return det;
}

double det2_par(double** matrix, int n)
{
    double det = 0;
    if (n==2) det = det2x2(matrix, n);

    else
    {
        double*** submatrixs = (double***)malloc((n)*sizeof(double**));
        for(int m1 = 0; m1 < n; m1++){
            submatrixs[m1] = (double**)malloc((n-1)*sizeof(double*));
            for(int m2 = 0; m2 < n-1; m2++)
                submatrixs[m1][m2] = (double*)malloc((n-1)*sizeof(double));
        }

        #pragma omp parallel for reduction(+:det)
        for (int i = 0; i < n; i++)
        {
            double x = matrix[0][i];
            double sign = (double)pow(-1.0, 2.0 + i);
            submatrix(matrix, submatrixs[i], n, i);
            // printf("Current submatrix:\n");
            printNxNSubMatrix(submatrixs[i], n-1);
            det += sign * x * det2_par(submatrixs[i], n-1);
        }
    }
    return det;
}


int main() {

    /*matrix = {{1.0, 4.0, 8.0, 8.0},
          {2.0, 2.0, 2.0, 8.0},
          {1.0, 2.0, 3.0, 4.0},
          {9.0, 8.0, 7.0, 6.0}};*/


    int i, j, n;
    FILE *fp;
    // read in file1
    fp = fopen("largerNxN50.txt", "r");
    fscanf(fp, "%i", &(n));

    // printf("n is %i \n", n);

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
    // printf("Input matrix:\n");
    omp_set_num_threads(2);
    // printNxNMatrix(matrix, n);
    // printf("Finding Determinants...\n");
    double t = omp_get_wtime();
    det2_par(matrix, n);
    printf("excution time is %0.5lf seconds\n", (omp_get_wtime()-t));

    // printf("Determinate is %f.", det2_par(matrix, n)); 
    return 0;
}