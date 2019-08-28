#include <stdio.h>
#include <mpi.h>

#define BUFFER_SIZE 50

int main (int argc, char *argv[])
{
  int rank, 
      size,
      int_size;
  
  char buffer[BUFFER_SIZE];

  MPI_File file;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Type_size(MPI_INT, &int_size);  

  MPI_File_open(MPI_COMM_WORLD, "hello_mpi.bin", MPI_MODE_WRONLY + MPI_MODE_CREATE, MPI_INFO_NULL, &file);

  sprintf(buffer, "(%d/%d) : Hello, World!\n", rank + 1, size);
  MPI_File_write_at(file, rank * BUFFER_SIZE, &buffer, BUFFER_SIZE, MPI_CHAR, 
MPI_STATUS_IGNORE);

  MPI_File_close(&file);

  MPI_Finalize();
  return 0;
}
