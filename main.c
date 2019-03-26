#include <omp.h>
#include <stdio.h>

int main() {
    int i,j,k;
//DEFINE STATIC MATRIX FOR TESTING
// 2 rows 1 column
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
    int c[2][2];
    const int dim = 2;
// BEGINNING OF Parallel STRUCTURE
#pragma omp parallel num_threads(2)
#pragma omp for schedule(static)
    for (i = 0; i < dim; i++) {
			 //printf("hello3\n");
        for (j = 0; j < dim; j++) {
            c[i][j] = 0;
						//printf("hello2\n");
						//#pragma omp for schedule(static)
            for (k = 0; k < dim; k++) {
                c[i][j] += a[i][k] * b[k][j];
							//	printf("hello1\n");
            }
        }
    }
	 #pragma omp barrier

    for(i = 0; i< dim; i++){
        for(j = 0; j < dim; j++) {
            printf("%d ",c[i][j]);
        }
        printf("\n");
    }
}
