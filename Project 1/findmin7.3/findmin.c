#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>

// computing the minimum entry in a list of integers, partial_list_size: size of each thread is stored in the variable and pointer to each thread partial list is passed as list_ptr
/* Global variable*/
void *find_min(void *list_ptr);
pthread_mutex_t minimum_value_lock;
int minimum_value; long partial_list_size;

int  Num_Threads;

int  nelems;


static double
mysecond()
{
    struct timeval    tp;
    struct timezone    tzp;
    int i = 0;
    
    i = gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

void *find_min(void *list_ptr)
{
    int *partial_list_pointer =NULL;
    int my_min=0;
    int i=0;
    partial_list_pointer = (int *) list_ptr;
    my_min = partial_list_pointer[0];
    for (i=0; i< partial_list_size; i++)
        
        if (partial_list_pointer[i]<my_min){
            my_min = partial_list_pointer[i];
        }
    //lock the mutex associated with minimum_valueand update the varibale as required
    
    
    pthread_mutex_lock(&minimum_value_lock);
    if (my_min < minimum_value){
        minimum_value=my_min;}
    //unlock the mutex
    pthread_mutex_unlock(&minimum_value_lock);
    pthread_exit(0);
    
    
}

int main(int argc, char **argv){
	
    int i=0;
    int *list = NULL;
    pthread_t *tids = NULL;
    //int ret = 10;
    long l = 0;
    long cur = 0;
    void *res = NULL;
    double start = 0.;
    double end = 0.;
	  srand(time(NULL));

    printf("enter the number of threads\n");
	 scanf("%d", &Num_Threads);

	  printf("enter the number of elements\n");
	 scanf("%d", &nelems);
    //declare and intialize data structure and list
    //minimum_value = MIN_INT;
    //int seed=100;
		//srand(time(NULL));

    // pthread_init();
    pthread_mutex_init(&minimum_value_lock, NULL);
    list = malloc(sizeof(int) * nelems);
    tids = malloc(sizeof(pthread_t) * Num_Threads);
    //initialize list, list-ptr and partail list size
    if( Num_Threads<1)
    {
        printf("error not enough threads ");
        return -1;
    }
    if(nelems <= Num_Threads)
    {
        printf("error");
        return -1;
    }
    minimum_value = INT_MAX;
    
    //srand(seed);

    for (l = 0; l < nelems; l++) {
        list[l] = rand() % 100;
    }
    // this spilts the threads.
    if (Num_Threads == 1) {
        partial_list_size = nelems;
    }
    else {
        partial_list_size = (nelems / (long)(Num_Threads)) + (nelems % (long)(Num_Threads));
    }
    
    start = mysecond();
    
    //create and join threads here
    for (i = 0; i < Num_Threads; i++) {
        if (pthread_create(&tids[i], NULL, &find_min, &list[cur]) != 0) {
            printf("Error : pthread_create failed on spawning thread %d\n", i);
            return -1;
        }
        cur += partial_list_size;
        
        /*
         * we do this check in order to ensure that our threads
         * down't go out of bounds of the list
         *
         */
        if ((cur + partial_list_size) > nelems) {
            cur = nelems - partial_list_size;
        }
    }
    
    /* join threads */
    for (i = 0; i < Num_Threads; i++) {
        if (pthread_join(tids[i], &res) != 0) {
            printf("Error : pthread_join failed on joining thread %d\n", i);
            return -1;
        }
    }
    end = mysecond();
    
    printf("Minimum value found: %d\n", minimum_value);
    printf("Runtime of %d threads = %f seconds\n", Num_Threads, (end - start));
    
    free(list);
    free(tids);
    tids = NULL;
    list = NULL;
    return (0);
}