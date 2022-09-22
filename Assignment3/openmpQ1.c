// C Program to find the minimum scalar product of two vectors (dot product)
#include<stdio.h>
#include <omp.h>
int threads = 4;


int sort(int arr[], int n)
{
	int i, j;
	#pragma omp parallel shared(arr) private(j)
	#pragma omp for schedule(dynamic)
	for (i = 0; i < n-1; i++)
		for (j = 0; j < n-i-1; j++)
			if (arr[j] > arr[j+1])
			{
				int temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
}

int sort_des(int arr[], int n)
{
	int i,j;
	#pragma omp parallel shared(arr) private(j)
	#pragma omp for schedule(dynamic)
	for (i = 0; i < n; ++i)
	{
		for (j = i + 1; j < n; ++j)
		{
			if (arr[i] < arr[j])
			{
				int a = arr[i];
				arr[i] = arr[j];
				arr[j] = a;
			}
		}
	}
}

int main()
{
	//fill the code;
	int i,tid,n,psum;
	printf("Enter Size of Arrays: ");
	scanf("%d",&n);
	int arr1[n], arr2[n];
	printf("Enter Elements of First Array:\n");
	for(i = 0; i < n ; i++)
	{
		scanf("%d",&arr1[i]);
	}
	printf("Enter Elements of Second Array:\n");
	for(i = 0; i < n ; i++)
	{
		scanf("%d",&arr2[i]);
	}
	sort(arr1, n);
	sort_des(arr2, n);
	int sum = 0;
	// making  threads 
	#pragma omp parallel private(i,tid,psum) num_threads(threads)
	{
		psum=0;
		tid = omp_get_thread_num();
		// running for loop on each thread
		#pragma omp for reduction(+:sum)
		for(int i=0; i<n; i++)
		{
			sum += arr1[i] * arr2[i];
			psum+=sum;
		}
		printf("Thread %d partial sum = %d\n",tid,psum);
	}
	printf("Sum: %d \n",sum);
	
	return 0;
}
