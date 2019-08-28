#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void srandom (unsigned seed);
double dboard (int darts);
#define DARTS 50000     /* number of throws at dartboard */
#define ROUNDS 10       /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[])
{
  double  homepi,         /* value of pi calculated by current task */
          pi,             /* average of pi after "darts" is thrown */
          avepi,          /* average pi value for all iterations */
          pirecv,         /* pi received from worker */
          pisum;          /* sum of workers pi values */
  int     i, n;

  int rank, // rank
      size, // #processes
      init, // initialization
      darts_per_process; // darts per process
  
  // initialize mpi environment
  if ((init = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
    printf("MPI environment initialization error.\n");
    MPI_Abort(MPI_COMM_WORLD, init);
  }

  // rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // random seed
  srandom(rank);
  // darts per process (master does remaining ones)
  darts_per_process = DARTS/size + (rank == MASTER ? DARTS%size : 0);

  avepi = 0;
  for (i = 0; i < ROUNDS; i++) {
    // compute pi (over darts per process)
    homepi = dboard(darts_per_process);
    // master - receive result from others and print result
    if (rank == MASTER) { // master - receive from slaves and compute avgpi
      MPI_Request *requests = malloc(sizeof(MPI_Request) * (size - 1));
      for (n = 1; n < size; n++) {
        // immediate receive from slaves
        MPI_Irecv(&pirecv, 1, MPI_DOUBLE, n, MASTER, MPI_COMM_WORLD, &requests[n - 1]);
      }
      homepi = dboard(darts_per_process);
      for (n = 1; n < size; n++) {
        MPI_Wait(&requests[n - 1], MPI_STATUS_IGNORE);
        homepi += pirecv;
      }
      pi = homepi/(double)size;
      // calculate average value of pi over all iterations
      avepi = ((avepi * i) + pi)/(i + 1);
      printf("After %8d throws, average value of pi = %10.8f\n", (DARTS * (i + 1)), avepi);
    } else { // slave - send result to master
        MPI_Ssend(&homepi, 1, MPI_DOUBLE, MASTER, MASTER, MPI_COMM_WORLD);
    }
  }

  // cleanup environment
  MPI_Finalize();
  return 0;
}



/******************************************************************************
* FILE: dboard.c
* DESCRIPTION:
*   Used in pi calculation example codes. 
*   See mpi_pi_send.c and mpi_pi_reduce.c  
*   Throw darts at board.  Done by generating random numbers 
*   between 0 and 1 and converting them to values for x and y 
*   coordinates and then testing to see if they "land" in 
*   the circle."  If so, score is incremented.  After throwing the 
*   specified number of darts, pi is calculated.  The computed value 
*   of pi is returned as the value of this function, dboard. 
*   Note:  the seed value for rand() is set in pi_send.f or pi_reduce.f. 
* AUTHOR: unknown
* LAST REVISED: 04/14/05 Blaise Barney
****************************************************************************/
/*
Explanation of constants and variables used in this function:
  darts       = number of throws at dartboard
  score       = number of darts that hit circle
  n           = index variable
  r           = random number between 0 and 1
  x_coord     = x coordinate, between -1 and 1
  x_sqr       = square of x coordinate
  y_coord     = y coordinate, between -1 and 1
  y_sqr       = square of y coordinate
  pi          = computed value of pi
*/


#include <stdio.h>
#include <stdlib.h>
#define sqr(x) ((x)*(x))
long random(void);

double dboard(int darts)
{
  double x_coord, y_coord, pi, r; 
  int score, n;
  unsigned int cconst;  /* must be 4-bytes in size */
  /*************************************************************************
   * The cconst variable must be 4 bytes. We check this and bail if it is
   * not the right size
   ************************************************************************/
  if (sizeof(cconst) != 4) {
    printf("Wrong data size for cconst variable in dboard routine!\n");
    printf("See comments in source file. Quitting.\n");
    exit(1);
  }
  cconst = 2 << (31 - 1);
  score = 0;

  /* "throw darts at board" */
  for (n = 1; n <= darts; n++) {
    /* generate random numbers for x and y coordinates */
    r = (double)random()/cconst;
    x_coord = (2.0 * r) - 1.0;
    r = (double)random()/cconst;
    y_coord = (2.0 * r) - 1.0;

    /* if dart lands in circle, increment score */
    if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
      score++;
  }

  /* calculate pi */
  pi = 4.0 * (double)score/(double)darts;
  return(pi);
} 
