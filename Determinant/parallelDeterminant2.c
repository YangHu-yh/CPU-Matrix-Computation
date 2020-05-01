// C program to find Deteminant of a matrix 
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h>
#include <omp.h>

// print matrix	
void printNxNMatrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // printf("%f, ",matrix[i][j]);
        }
        // printf("\n");
    }
    // printf("\n");
}

// Function to get determinant of matrix   
int determinantOfMatrix(double** matrix, int n, int p)   
{   
    int index = 0;
    double num1,num2,det = 1,total = 1; // Initialize result   
    
    // temporary array for storing row   
    double* temp = (double*)malloc((n+1)*sizeof(double));   
         
    //loop for traversing the diagonal elements  
    for(int i = 0; i < n; i++)    
    {  
        index = i; // initialize the index   
            
        // stop when there is non zero value   
        while(matrix[index][i] == (double)0 && index < n) {   
            index++;       
        }   
        if(index == n){   
            // the determinat of matrix as zero   
            continue;       
        }   
        // printf("index = %d when i = %d.\n",index,i);
        if(index != i){   
            //loop for swaping the diagonal element row and index row 
            #pragma omp parallel for schedule(dynamic) num_threads(p)
                for(int j = 0; j < n; j++){   
                    // swap(mat[index][j],mat[i][j]);      
                    double temp0 = matrix[index][j];
                    matrix[index][j] = matrix[i][j];
                    matrix[i][j] = temp0;
                }  
            //determinant sign changes when we shift rows   
            //go through determinant properties   
            det = det*pow(-1,index-i);     
        }
        // printf("det = %f",det);   
        #pragma omp parallel for num_threads(p)
            for(int j = 0; j < n; j++) //storing the values of diagonal row elements   
                temp[j] = matrix[i][j];      
        
        // printf("matrix before traversing:\n");
        // printfNxNMatrix(matrix, n);
        // #pragma omp parallel for
        {
            for(int j = i+1; j < n; j++)//traversing every row below the diagonal element 
            {   
                num1 = temp[i]; //value of diagonal element   
                num2 = matrix[j][i]; //value of next row element   
                    
                //traversing every column of row   
                // and multiplying to every row  
                #pragma omp parallel for num_threads(p)
                    for(int k = 0; k < n; k++)   
                    {   
                        //multiplying to make the diagonal   
                        // element and next row element equal   
                        matrix[j][k] = (num1 * matrix[j][k]) - (num2 * temp[k]);
                        // printf("matrix[j=%d][k=%d] = (num1=%f * matrix[j][k]) - (num2=%f * temp[k]=%f) = %f;\n",j,k,num1,num2,temp[k],matrix[j][k]);
                        // // printfNxNMatrix(matrix, n);
                    }
            }
        }
        // printf("matrix after traversing and get total = %f:\n",total);
        // printfNxNMatrix(matrix, n);
    }
    total=1;
    //mulitplying the diagonal elements to get total   
    #pragma omp parallel for reduction(*:total)
    for(int i = 0; i < n-1; i++){
        int power = n-i-1;
        total *= pow(matrix[i][i],power);
    }           
    // printf("now total = %f:\n",total);

    //mulitplying the diagonal elements to get determinant   
    #pragma omp parallel for reduction(*:det)
    for(int i = 0; i < n; i++)   
        det = det * matrix[i][i];   

    return (det/total); //Det(kA)/k=Det(A);   
    }   
	
// Driver code 
int main() {

    int i, j, n;
    FILE *fp;
    // read in file1
    fp = fopen("largerNxN1.txt", "r");
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
    printNxNMatrix(matrix, n);
    // printf("Finding Determinants...\n");
    double t = omp_get_wtime();
    determinantOfMatrix(matrix, n, 6);
    printf("excution time is %0.5lf seconds\n", (omp_get_wtime()-t));
    // printf("Input matrix:\n");
    // printfNxNMatrix(matrix, n);
    // printf("finding determinant....\n");
	printf("Determinant of the matrix is : %d\n", determinantOfMatrix(matrix, n, 4)); 

// /*int mat[N][N] = {{6, 1, 1}, 
// 					{4, -2, 5}, 
// 					{2, 8, 7}}; */ 
	
// 	int mat[N][N] = {{1, 0, 2, -1}, 
// 					{3, 0, 0, 5}, 
// 					{2, 1, 4, -3}, 
// 					{1, 0, 5, 0} 
// 					}; -> det = 30
		
	return 0; 
		
} 
