//Fibonacci Series using Recursion

#include<stdio.h>

int fib(int n)

{
	int i,j;
	if(n<2)
		return n;
	else
	{
		#pragma omp task shared(i) firstprivate(n)
		i = fib(n-1);
		
		#pragma omp task shared(j) firstprivate(n)
		j = fib(n-2);
		
		#pragma omp taskwait
		return i+j;
	}
}

int main ()
{
	int n = 15;
	#pragma omp parallel shared(n)
	{
		#pragma omp single
		printf("%d\n", fib(n));
	}
//	getchar();
	return 0;
}
