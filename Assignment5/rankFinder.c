#include "mpi.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
    MPI_Init( &argc, &argv );
    int rank;
    MPI_Group group;

    MPI_Comm_group(MPI_COMM_WORLD, &group);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("Rank: %d, Group: %d \n", rank, group);
    MPI_Finalize();

    return 0;
}