#include <stdio.h>
#include <mpi.h>

#define MASTER 0

int main (int argc, char *argv[])
{
  const int N=300;
  int i,
      target;
  int b[N], local_b[N];
  int b_start,
      b_end;
  FILE *input_file,
       *output_file;
  char output_filename[20];
  int rank, // process index
      size, // #processes
      init; // initialization

  /* File b.data has the target value on the first line
     The remaining 300 lines of b.data have the values for the b array */
  input_file = fopen("b.data","r" ) ;

  /* read in target */
  fscanf(input_file, "%d", &target);
  
  /* read in b array */
  for(i = 0; i < N; i++) {
    fscanf(input_file, "%d", &b[i]);
  }
  fclose(input_file);

  // initialize mpi environment
  if ((init = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
    printf("MPI initialization error.\n");
    MPI_Abort(MPI_COMM_WORLD, init);
  }

  // retrieve rank and size
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // broadcast vector
  MPI_Scatter(&b, N, MPI_INT, &local_b, N, MPI_INT, MASTER, MPI_COMM_WORLD);
  // MPI_Scatter(&b, N, MPI_INT, &local_b, N, MPI_INT, MASTER, MPI_COMM_WORLD);
    
  // create process-local output file
  sprintf(output_filename, "found.data_%d", rank);
  output_file = fopen(output_filename, "w");
  // initialize b_start
  b_start = rank * (N / size); // 0[0], 1[N/size], [2N/size]
  b_end = rank == size - 1 ? N : b_start + (N/size); // 0[N/size]

  // search target
  for (i = b_start; i < b_end; i++) {
    
    if (b[i] == target) {
      fprintf(output_file, "%d\n", i + 1);
    }
  }
  fclose(output_file);

  MPI_Finalize();
  return 0;
}