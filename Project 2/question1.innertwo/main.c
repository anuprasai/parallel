#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>


#define MAXN 2000/* Max value of N */
volatile float A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
int Numthreads;
int N;
static double  
mysecond()
{
	struct timeval	tp;
	struct timezone	tzp;
	int i = 0;

	i = gettimeofday(&tp, &tzp);
	return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}



int main() {

	 printf("enter the matrix size\n");
	 scanf("%d", &N);

	  printf("enter the number of threads\n");
	 scanf("%d", &Numthreads);
	double start = 0.;
	double end = 0.;
    int i,j,k;
//DEFINE STATIC MATRIX FOR TESTING
/*// 2 rows 1 column
    const int a[2][2] = {
    {1, 2},
    {3, 4}
    };
// 1 rows 1 column
    const int b[2][2] = {
    {1, 10},
    {2, 20}
    };
// 2 rows 1 column
    int c[2][2];*/
srand(time(NULL));
//int rand_num = rand() % 100;
for (int i = 0; i < N; i++) { 
        for (int j = 0; j < N; j++) { 
            A[i][j] = rand() % 100; 
	          //printf("%f ",A[i][j]);

            B[i][j] = rand() % 100; 
					  //printf("%f ",B[i][j]);

        } 
    } 
if (N < 8){
printf("printing matrix A\n");
for (int row=0; row<N; row++)
{
    for(int columns=0; columns<N; columns++)
        {
         printf("%f     ", A[row][columns]);
        }
    printf("\n");
 }
}
printf("printing matrix B\n");
if (N < 8){
for (int row=0; row<N; row++)
{
    for(int columns=0; columns<N; columns++)
        {
         printf("%f     ", B[row][columns]);
        }
    printf("\n");
 }}

/*for (int i = 0; i < MAXN; i++) { 
        for (int j = 0; j < MAXN; j++)  
            printf("%5.2f%s", A[i][j], ", " ";\n\t");
      
    } 
*/
    const int dim = N;
// BEGINNING OF Parallel STRUCTURE
	start = mysecond();

//#pragma omp parallel num_threads(9)
//#pragma omp for schedule(static)
    for (i = 0; i < dim; i++) {
			 //printf("hello3\n");
			 #pragma omp parallel num_threads(Numthreads)
				#pragma omp for schedule(static)
        for (j = 0; j < dim; j++) {
            C[i][j] = 0;
						//printf("hello2\n");
						//#pragma omp parallel num_threads(9)
						//#pragma omp for schedule(static)
            for (k = 0; k < dim; k++) {
                C[i][j] += A[i][k] * B[k][j];
							//	printf("hello1\n");
            }
        }

    }
	 //#pragma omp barrier
			end = mysecond();

  	printf("printing matrix C\n");
    for(i = 0; i< dim; i++){
        for(j = 0; j < dim; j++) {
            printf("%f ",C[i][j]);
        }
        printf("\n");
    }
			printf("Runtime of threads = %f seconds\n", (end - start));

}
