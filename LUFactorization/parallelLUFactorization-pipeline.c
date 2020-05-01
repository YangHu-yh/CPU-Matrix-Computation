// C Program to decompose a matrix into 
// lower and upper traingular matrix 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <limits.h>
#include <omp.h>

int n = 4;

struct record_s {
    double val;
    long prod;
    struct record_s* next;
};

struct buf_list {
    struct record_s* head_p;
    struct record_s* tail_p;
};

struct buf_list buff[4];
int producers_done[4];

struct record_s* Dequeue(long thread) {
    struct record_s* rec_p = malloc(sizeof(*rec_p));
    
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
    struct record_s* rec_p;
    double data;

    while (producers_done[thread] < 1 || buff[thread].head_p != NULL) {
        #pragma omp critical (queue)
        {                
            rec_p = Dequeue(thread);
        }
        if (rec_p != NULL) {
            data = rec_p -> val;
            free(rec_p);
            return data;
        }

    }
    return 0.;
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
    struct record_s *rec_p;

    rec_p = Create_record(thread,data);
    #pragma omp critical(queue)
    {
        Enqueue(thread, rec_p);
    }
    #pragma omp critical(done)
    producers_done[thread]++;
}

// print matrix	to file
void printNxNMatrix(double** matrix, int n, FILE *fp) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(fp, "%lf  ", matrix[i][j]);
        }
        fprintf(fp, "\n");
    }
}
// multiply two matrix
int multiply(int m1, int m2, double** lower, 
			int n1, int n2, double** upper,
            double** matrix, int T, FILE *fp)
{ 
	int x, i, j; 
    double** res_matrix = (double**)malloc(m1*sizeof(double*));
    for (i = 0; i<m1; i++){
        res_matrix[i] = (double*)malloc(n2*sizeof(double));
    }


    int ii;
    #pragma omp parallel num_threads(T)
    {
        #pragma omp for
        {
            for (ii=0; ii < m1; ii++) {
                int jj, kk;
                for (jj=0; jj < n2; jj++){
                    for (kk=0; kk<n1; kk++){
                        res_matrix[ii][jj] += (lower[ii][kk]) * (upper[kk][jj]);
                    }
                }
            }
        }
        #pragma omp barrier
    }

	for (i = 0; i < m1; i++) 
	{ 
		for (j = 0; j < n2; j++) 
		{ 
			if(llabs((long long)matrix[i][j] - (long long)res_matrix[i][j]) > 0.0001){
                printNxNMatrix(res_matrix, n, fp);
                return 0;
            } 
		}
	} 
    return 1;
}

  
void luDecomposition(double** matrix, int n, int p) 
{ 
    // Setting lower and upper...\n");
    double** lower = (double**)malloc(n*sizeof(double*));
    double** upper = (double**)malloc(n*sizeof(double*));
    for (int i = 0; i<n; i++){
        lower[i] = (double*)malloc((n)*sizeof(double));
        upper[i] = (double*)malloc((n)*sizeof(double));
    }
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n ; j++){
            lower[i][j] = matrix[i][j];
            upper[i][j] = matrix[i][j];
        }
    }
    int i,j,k,row;
    omp_set_num_threads(p);
    // Decomposing matrix into Upper and Lower 
    // triangular matrix 
    double t = omp_get_wtime();
    #pragma omp parallel private(k, i, j, row) shared(matrix, lower, upper) num_threads(p)
    {
        long threadID = omp_get_thread_num();
        int blockSize = n/p;
        if (threadID != 0) {
            for (k = 0; k < n-1; k++) {
                row = Get(threadID);
                Put(threadID+1, row);
                for (i = threadID * blockSize; i < threadID * blockSize + blockSize; i++) {
                    if (row < i) { 
                        lower[i][row] = upper[i][row] / upper[row][row];
                    }
                }

                for (i = threadID * blockSize; i < threadID * blockSize + blockSize; i++) {
                    for (j = k; j < n; j++) {
                        if (row < i) {
                            upper[i][j] = upper[i][j] - lower[i][row] * upper[row][j];
                        }
                    }
                }
            }
            // thread 0
        } else {
            for (k = 0; k < n-1; k++) {
                Put(threadID+1,k);
                for (i = k+1; i < threadID * blockSize + blockSize; i++) {
                    lower[i][k] = upper[i][k]/upper[k][k];
                }
                for (i = k+1; i < threadID * blockSize + blockSize; i++) {
                    for (j = k+1; j < n; j++) {
                        upper[i][j] = upper[i][j] - lower[i][k] * upper[k][j];
                    }
                } 
            }
        }
    }
    for (int j = 0; j < n; j++) { 
        for(int i = 0; i < n; i++){
            if(i > j){
                upper[i][j] = 0.;
            }else{
                lower[i][j] = 0.;
            }
        }
        lower[j][j] = 1.;
    } 
    // putting lower matrix into file
    FILE *fp;
    fp = fopen("lower_matrix.txt", "w");
    printNxNMatrix(lower, n, fp);
    fclose(fp);
    // putting upper matrix into file
    fp = fopen("upper_matrix.txt", "w");
    printNxNMatrix(upper, n, fp);
    fclose(fp);

    // check correctness
    fp = fopen("multiplication_result.txt","w");
    if (multiply(n, n, lower, n, n, upper, matrix, 4, fp)){
        printf("Pass the test.\n");
    }
    fclose(fp);
} 
  
// Driver code 
int main() 
{ 
    int i, j;
    FILE *fp;
    // read in file1
    fp = fopen("file_six_six.txt", "r");
    fscanf(fp, "%i", &(n));

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
    luDecomposition(matrix, n, 2); 
    return 0; 
} 