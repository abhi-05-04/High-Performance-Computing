#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int rank;
    int num;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status;

    double d = 483048.0;
    int tag = 1;

    //calculating next rank 
    int rank_next = (rank + 1) % num;
    //prev process rank
    int rank_prev = rank == 0 ? num - 1 : rank - 1;

    if (num % 2 == 0)
    {
        printf("Rank %d: sending  to %d\n", rank, rank_next);
        MPI_Send(&d, 1, MPI_DOUBLE, rank_next, tag, MPI_COMM_WORLD);

        printf("Rank %d: receiving from %d\n", rank, rank_prev);
        MPI_Recv(&d, 1, MPI_DOUBLE, rank_prev, tag, MPI_COMM_WORLD, &status);
    }
    else
    {
        printf("Rank %d: receiving from %d\n", rank, rank_prev);
        MPI_Recv(&d, 1, MPI_DOUBLE, rank_prev, tag, MPI_COMM_WORLD, &status);

        printf("Rank %d: sending  to %d\n", rank, rank_next);
        MPI_Send(&d, 1, MPI_DOUBLE, rank_next, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}