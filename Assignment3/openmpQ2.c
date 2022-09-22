#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 800

int main()
{
	int tid, nthreads, i, j;
	int a[N][N], b[N][N], c[N][N];
	omp_set_num_threads(8); // number of threads
	

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			a[i][j] = i + j;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			b[i][j] = i + j;
	double time = omp_get_wtime();
	#pragma omp parallel shared(a, b, c, nthreads) private(tid, i, j)
	{

	#pragma omp for
		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
				c[i][j] = a[i][j] + b[i][j];
		}
	}
	// printf("\nResultant Matrix:\n");
	// for (i=0; i<N; i++)
	// {
	// 	for (j=0; j<N; j++)
	// 		printf("%d  ", c[i][j]);
	// 	printf("\n");
	// }

	printf("\nDone In %f Seconds", omp_get_wtime() - time);
	printf("\n Using %d Threads & %d matrix length\n", omp_get_max_threads(),N);

	return (0);
}