#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    int rank,
        size;

    MPI_Request request;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int data = rand();
    if (rank > 0) {
        // immediate receive and store request
        MPI_Irecv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &request);
        // wait receive to complete
        MPI_Wait(&request, MPI_STATUS_IGNORE); // commenting out will introduce race condition over 'data'
        // add one to value
        data += 1;
        // print value
        printf("Process %d : %d.\n", rank, data);
    }
    if (rank < size - 1) {
        // send value
        MPI_Isend(&data, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
    }
    MPI_Finalize();
    return 0;
}