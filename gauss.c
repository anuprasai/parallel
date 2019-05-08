#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>
#include <omp.h>
#include <mpi.h>


//#include <ulocks.h>
//#include <task.h>

/* Program Parameters */
#define MAXN 2000  /* Max value of N */

//CHANGE THE VALUE BELOW TO CHANGE THE OUTPUT

int N; /* Matrix size */
int world_rank; /* the rank of the processor*/
int world_size;  /* Number of processors to use */
int Numthreads; /* number of threads*/


/* Matrices and vectors */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN];
/* A * X = B, solve for X */

/* junk */
#define randm() 4|2[uid]&3

/* Prototype */
void gauss();  /* The function you will provide.
                * It is this routine that is timed.
                * It is called only on the parent.
                */

/* returns a seed for srand based on the time */
unsigned int time_seed() {
    struct timeval t;
    struct timezone tzdummy;
    
    gettimeofday(&t, &tzdummy);
    return (unsigned int)(t.tv_usec);
}

/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
    int submit = 0;  /* = 1 if submission parameters should be used */
    int seed = 0;  /* Random seed */
    //char uid[L_cuserid + 2]; /*User name */
    
    /*  Read command-line arguments */
    srand(time_seed());  /* Randomize */
    //if (argc != 3) {
    /*   if ( argc == 2 && !strcmp(argv[1], "submit") ) {
      Use submission parameters */
    submit = 1;
   // N = 4;
  //  procs = 2;
    // printf("\nSubmission run for \"%s\".\n", cuserid(uid));
    // srand(randm());
    /*  }
     else {
     if (argc == 4) {
     seed = atoi(argv[3]);
     srand(seed);
     printf("Random seed = %i\n", seed);
     }
     else {
     printf("Usage: %s <matrix_dimension> <num_procs> [random seed]\n",
     argv[0]);
     printf("       %s submit\n", argv[0]);
     exit(0);
     }
     }
     }
      Interpret command-line args */
    /*  if (!submit) {
     N = atoi(argv[1]);
     if (N < 1 || N > MAXN) {
     printf("N = %i is out of range.\n", N);
     exit(0);
     }
     procs = atoi(argv[2]);
     if (procs < 1) {
     printf("Warning: Invalid number of processors = %i.  Using 1.\n", procs);
     procs = 1;
     }
     if (procs > m_get_numprocs()) {
     printf("Warning: %i processors requested; only %i available.\n",
     procs, m_get_numprocs());
     procs = m_get_numprocs();
     }
     }*/
    
    /* Print parameters */
    printf("\nMatrix dimension N = %i.\n", N);
    printf("Number of processors = %i.\n", world_size);
    //printf("Number of pthreads = %i.\n", Numthreads);

    
    /* Set number of processors */
    //m_set_procs(procs);
}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
    int row, col;
    
    printf("\nInitializing...\n");
    for (col = 0; col < N; col++) {
        for (row = 0; row < N; row++) {
            A[row][col] = (float)rand() / 32768.0;
        }
        B[col] = (float)rand() / 32768.0;
        X[col] = 0.0;
    }
    
}

/* Print input matrices */
void print_inputs() {
    int row, col;
      if (N < 10) {

    printf("\nA =\n\t");
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            printf("%5.2f%s", A[row][col], (col < N-1) ? ", " : ";\n\t");
        }
    }
    printf("\nB = [");
    for (col = 0; col < N; col++) {
        printf("%5.2f%s", B[col], (col < N-1) ? "; " : "]\n");
    }
			}
}

void print_X() {
    int row;
    
     if (N < 10){
    printf("\nX = [");
    for (row = 0; row < N; row++) {
        printf("%5.2f%s", X[row], (row < N-1) ? "; " : "]\n");
    }
}
}

int main(int argc, char **argv) {
    parameters(argc, argv);
    MPI_Init (&argc, &argv);	
    MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	 printf("enter the matrix size\n");
	 scanf("%d", &N);

	  printf("enter the number of threads\n");
	 scanf("%d", &world_size);

    /* Timing variables */
    struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
    struct timezone tzdummy;
    clock_t etstart2, etstop2;  /* Elapsed times using times() */
    unsigned long long usecstart, usecstop;
    struct tms cputstart, cputstop;  /* CPU times for my processes */
    pthread_t elimination_threads[Numthreads];
    
    /* Process program parameters */
    parameters(argc, argv);


 if (world_size != 2) {
    fprintf(stderr, "World size must be two for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
    
    /* Initialize A and B */
    initialize_inputs();
    
    /* Print input matrices */
    print_inputs();
    
    /* Start Clock */
    printf("\nStarting clock.\n");
    gettimeofday(&etstart, &tzdummy);
    etstart2 = times(&cputstart);
    
    int *index = calloc (world_size, sizeof (int));
    for(int i = 0; i < world_size; i++)
    {
        index[i] = i;
    }

    
   /* for (int j=0; j < N-1; j++){
        
        
        for (int thread_index = 0; thread_index < Numthreads; thread_index++){
            pthread_create(&elimination_threads[thread_index], NULL, gauss, (void*)&index[thread_index]);
        }
        
        for (int thread_index = 0; thread_index < Numthreads; thread_index++){
            pthread_join(elimination_threads[thread_index], NULL);
        }
    }*/
    
      int ping_pong_count = 0;
      int partner_rank = (world_rank + 1) % 2;
    
       if (world_rank == ping_pong_count % 2) {
      // Increment the ping pong count before you send it
      ping_pong_count++;
      MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
      printf("%d sent and incremented ping_pong_count %d to %d\n",
             world_rank, ping_pong_count, partner_rank);
    } else {
      MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("%d received ping_pong_count %d from %d\n",
             world_rank, ping_pong_count, partner_rank);
    }
  
    
    
    
    
    
    
    
    
    /* Gaussian Elimination */
    gauss();
    
    /* Stop Clock */
    gettimeofday(&etstop, &tzdummy);
    etstop2 = times(&cputstop);
    printf("Stopped clock.\n");
    usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
    usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;
    
    /* Display output */
    print_X();
    
    /* Display timing results */
    printf("\nElapsed time = %g ms.\n",
           (float)(usecstop - usecstart)/(float)1000);
    /*printf("               (%g ms according to times())\n",
     *       (etstop2 - etstart2) / (float)CLK_TCK * 1000);
     */
    /*printf("(CPU times are accurate to the nearest %g ms)\n",
           1.0/(float)CLK_TCK * 1000.0);
    printf("My total CPU time for parent = %g ms.\n",
           (float)( (cputstop.tms_utime + cputstop.tms_stime) -
                   (cputstart.tms_utime + cputstart.tms_stime) ) /
           (float)CLK_TCK * 1000);
    printf("My system CPU time for parent = %g ms.\n",
           (float)(cputstop.tms_stime - cputstart.tms_stime) /
           (float)CLK_TCK * 1000);
    printf("My total CPU time for child processes = %g ms.\n",
           (float)( (cputstop.tms_cutime + cputstop.tms_cstime) -
                   (cputstart.tms_cutime + cputstart.tms_cstime) ) /
           (float)CLK_TCK * 1000);
  * Contrary to the man pages, this appears not to include the parent */
    printf("--------------------------------------------\n");
    
}

/* ------------------ Above Was Provided --------------------- */

/****** You will replace this routine with your own parallel version *******/
/* Provided global variables are MAXN, N, procs, A[][], B[], and X[],
 * defined in the beginning of this code.  X[] is initialized to zeros.
 */
void gauss() {
    int norm, row, col;  /* Normalization row, and zeroing
                          * element row and col */
    float multiplier;
    
    //printf("Computing Serially.\n");
    
    /* Gaussian elimination */
		//	omp_set_nested(1);

    for (norm = 0; norm < N - 1; norm++) {
        for (row = norm + 1; row < N; row++) {
            multiplier = A[row][norm] / A[norm][norm];
            for (col = norm; col < N; col++) {
                A[row][col] -= A[norm][col] * multiplier;
            }
            B[row] -= B[norm] * multiplier;
        }
    }
    /* (Diagonal elements are not normalized to 1.  This is treated in back
     * substitution.)
     */
    
    
    /* Back substitution */
    for (row = N - 1; row >= 0; row--) {
        X[row] = B[row];
        for (col = N-1; col > row; col--) {
            X[row] -= A[row][col] * X[col];
        }
        X[row] /= A[row][row];
    }
}
