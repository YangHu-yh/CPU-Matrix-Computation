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

struct record_s {
    double val;
    long prod;
    struct record_s* next;
};

struct buf_list {
    struct record_s* head_p;
    struct record_s* tail_p;
};

struct buf_list *buff;
int *producers_done;

struct record_s* Dequeue(long thread) {
    struct record_s* rec_p;// = malloc(sizeof(*rec_p));
    
    if (buff[thread].head_p == NULL) {
        return NULL;
    } else if (buff[thread].head_p == buff[thread].tail_p) {
        rec_p = buff[thread].head_p;
        buff[thread].head_p = buff[thread].tail_p = NULL;
    } else {
        rec_p = buff[thread].head_p;
        buff[thread].head_p = buff[thread].head_p->next;
    }
    return rec_p;
}

double Get(long thread) {
    struct record_s* rec_p;// = malloc(sizeof(*rec_p));
    double data;

    while (producers_done[thread] < 1 || buff[thread].head_p != NULL) {
        #pragma omp critical (queue)
        {                
            rec_p = Dequeue(thread);

        }
        if (rec_p != NULL) {
        #pragma omp critical(done)
        {
            producers_done[thread] -= 1;
        }
            data = rec_p -> val;
            free(rec_p);
            return data;
        }
    }
}

struct record_s* Create_record(long thread, double data) {
    struct record_s* rec_p = malloc(sizeof(*rec_p));
    rec_p->next=NULL;
    rec_p->prod=thread;
    rec_p->val=data;
    return rec_p;
}

void Enqueue(long thread, struct record_s* rec_p) {
    if (buff[thread].tail_p == NULL) {
        buff[thread].head_p = rec_p;
    } else {
        buff[thread].tail_p->next = rec_p;
    }
    buff[thread].tail_p = rec_p;
}

void Put(long thread, double data) {
    struct record_s *rec_p = malloc(sizeof(*rec_p));

    rec_p->next=NULL;
    rec_p->prod=thread;
    rec_p->val=data;

    #pragma omp critical(queue)
    {
        Enqueue(thread, rec_p);
    }
    #pragma omp critical(done)
    {
        producers_done[thread]++;
    }
}

void paralle_gaussian_elimination(double **matrix, int n, int p) {
    int blockSize = n/p;
    for (int offset = 0; offset <= n - blockSize; offset+=blockSize) {
        int i,j,k,row,count;
        omp_set_num_threads(p);
        #pragma omp parallel private(i,j,k,row,count) shared(matrix)
        {
            long threadID = omp_get_thread_num();
            if (threadID != 0) { 
                for (count = 0; count < blockSize; count++) {
                    
                    row = Get(threadID);
                    Put(threadID+1, row);
                    for (i = threadID * blockSize; i < threadID * blockSize + blockSize; i++) {

                        if (row < i) {
                            matrix[i][row] = matrix[i][row]/matrix[row][row];
                            for (j = row+1; j < n+1; j++) {
                                matrix[i][j] = matrix[i][j] - matrix[i][row]*matrix[row][j];
                            }
                        }
                    }
                }
            // thread 0
            } else {
                for (count = 0; count < blockSize; count++) {
                    long k = count + offset;
                    Put(threadID+1,k);
                    for (i = k+1; i < blockSize; i++) {
                        for (int j = k+1; j < n+1; j++) {
                            matrix[i][j] -= (matrix[i][k] / matrix[k][k]) * matrix[k][j];
                        }                    
                    }    
                }
            }
        }
    }
}

void parallel_backward_substitution(double **matrix, int n) { 
    for (int i = n - 1; i >= 0; i--) {
        #pragma omp for schedule(static) 
        for (int j = n - 1; j > i; j--) {
            matrix[i][n] -= matrix[i][j] * matrix[j][n];
        }
        #pragma omp single 
        matrix[i][n] = matrix[i][n] / matrix[i][i];
    }
}

void main(int argc, char *argv[]) {
    int i, j, n, threads;
    char *input_file, *output_file;
    FILE *fp;
    clock_t start, end;

    input_file = argv[1];
    output_file = argv[2];
    int p = atoi(argv[3]);

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

    buff = (struct buf_list*) malloc((n+1)*sizeof(struct buf_list));
    producers_done = malloc((n+1)*sizeof(int));

    for (int i = 0; i < n + 1; i++) {
        buff[i].head_p = NULL;
        buff[i].tail_p = NULL;
        producers_done[i] = 0;
    }
 
    start = clock();
    paralle_gaussian_elimination(matrix,n,p);
    parallel_backward_substitution(matrix,n);
    end = clock();

    double runtime = (double) (end - start) / CLOCKS_PER_SEC;
    printf("parallel linear time (pipeline) is %lf: \r\n", runtime);

    fp = fopen(output_file, "w");
    printSolutions(matrix,n,fp); 
    fclose(fp);

    free(matrix);
    free(buff);
    free(producers_done);
}