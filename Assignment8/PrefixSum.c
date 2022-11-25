#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	
	MPI_Status status ;   /* return status for receive */
	int value;
	
	/* start up MPI */
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	int prefix_arr[p]; 
	
	/* getting input and scatter values */
	if(my_rank == 0)
	{
		int i;
		for(i = 0; i < p; ++i)
			prefix_arr[i] = i + 1;
	}
	
	double start = MPI_Wtime();
	
	//all call scatter
	MPI_Scatter(prefix_arr, 1, MPI_INT, &value, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	/*
	prefix sum:
	repeat log n times
	each time, if we are the chosen one, we receve a value from someone and add to ours
	otherwise, we send to the chosen one
	*/
	int i;
	int logn = log2(p);
	for(i = 0; i <= logn; i++)
	{
		int lower_bound = pow(2,i);
		int upper_bound = p - lower_bound;
		
		if(upper_bound < lower_bound)
			upper_bound = lower_bound;
		
		if(my_rank < lower_bound)
		{
			int send = (int) (my_rank + pow(2,i));
			if(send >= p)
				continue;
			
//			printf("%d sending to %d\n", my_rank, (int) (my_rank+pow(2,i)));
			MPI_Send(&value, 1, MPI_INT, (int) (my_rank+pow(2,i)), 0, MPI_COMM_WORLD);
		}
		else if(my_rank >= upper_bound)
		{
			int recv = (int) (my_rank - pow(2,i));
			if(recv >= p)
				continue;
			
			int recv_value;
//			printf("%d receving..\n", my_rank);
			MPI_Recv(&recv_value, 1, MPI_INT, (my_rank - pow(2,i)), 0, MPI_COMM_WORLD, &status);
			value += recv_value;
		}
		else
		{
			int send = (int) (my_rank + pow(2,i));
			int recv = (int) (my_rank - pow(2,i));
			if(send >= p || recv >= p)
				continue;
			
//			printf("%d sending to %d\n", my_rank, (int) (my_rank+pow(2,i)));
			MPI_Send(&value, 1, MPI_INT, (int) (my_rank+pow(2,i)), 0, MPI_COMM_WORLD);
			
//			printf("%d receving..\n", my_rank);
			int recv_value;
			MPI_Status status;
			MPI_Recv(&recv_value, 1, MPI_INT, (my_rank - pow(2,i)), 0, MPI_COMM_WORLD, &status);
			value += recv_value;
		}
	}
	
	//after algorithm, each processor hols its own prefix sum
	//we gather at rank
	int gather[p];
	MPI_Gather(&value, 1, MPI_INT, gather, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(my_rank == 0)
	{
		double end = MPI_Wtime();
		
		printf("\nPerfix Sum Execution Time: %f\n", end - start);
	}
	
	/* shut down MPI */
	MPI_Finalize(); 
	
	return 0;
}
