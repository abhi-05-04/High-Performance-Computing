#include "mpi.h"
#include <stdio.h>

#define localSize 1000

int local[1000]; // to store the subarray data comming from process 0;

int main(int argc, char **argv)
{
    int rank;
    int num;

    int n = 10;
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int per_process, elements_received;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status;

    // process with rank 0 will divide data among all processes and add partial sums to get final sum
    if (rank == 0)
    {
        int index, i;

        per_process = n / num;

        if (num > 1) // if more than 1 processes available
        {
            //divide array data among processes
            for (i = 1; i < num - 1; i++)
            {
                //calculating first index of subarray that need to be send to ith process
                index = i * per_process;

                //send no of elements and subarray of that lenght to each process
                MPI_Send(&per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&arr[index], per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            // for last process send all remaining elements
            index = i * per_process;
            int ele_left = n - index;

            MPI_Send(&ele_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[index], ele_left, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // add numbers on process with rank 0
        int sum = 0;
        for (int i = 0; i < per_process; i++)
        {
            sum += arr[i];
        }

        // add all partial sums from all processes
        int tmp;
        for (int i = 1; i < num; i++)
        {
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            int sender = status.MPI_SOURCE;

            sum += tmp;
        }

        printf("Sum of array = %d\n", sum);
    }
    else // if rank of process is not 0, then receive elements and calculate partial sums
    {
        // receive no of elements and elements form process 0 and store them on local array
        MPI_Recv(&elements_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        MPI_Recv(&local, elements_received, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // calculate partial local sum
        int partial_sum = 0;
        for (int i = 0; i < elements_received; i++)
        {
            partial_sum += local[i];
        }

        //send calculated partial sum to process with rank 0
        MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}