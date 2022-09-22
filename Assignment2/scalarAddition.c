#include <stdlib.h>   //malloc and free
#include <stdio.h>    //printf
#include <omp.h>      //OpenMP

// Very small values for this simple illustrative example
#define ARRAY_SIZE 32  //Size of arrays whose elements will be added together.
#define NUM_THREADS 8    //Number of threads to use for vector addition.

int main () 
{
	// elements of arrays a and b will be added
	// and placed in array c
	int * a;
	int  b=10; 
	int * c;
        
	int n = ARRAY_SIZE;                 // number of array elements
	int n_per_thread;                   // elements per thread
	int total_threads = NUM_THREADS;    // number of threads to use  
	int i;       // loop index
        
        // allocate spce for the arrays
        a = (int *) malloc(sizeof(int)*n);
	//b = (int *) malloc(sizeof(int)*n);
	c = (int *) malloc(sizeof(int)*n);

        // initialize arrays a and b with consecutive integer values
        for(i=0; i<n; i++) {
            a[i] = i;
        }
        
        
	// Additional work to set the number of threads.
	// We hard-code to 4 for illustration purposes only.
	omp_set_num_threads(total_threads);
	
	// determine how many elements each process will work on
	n_per_thread = n/total_threads;
	
        // Compute the vector-scalar addition

	#pragma omp parallel for shared(a, b, c) private(i) schedule(static, n_per_thread)
        for(i=0; i<n; i++) {
		c[i] = a[i]+b;
		// Show who works on what for this small example
		printf("Thread %d works on element%d\n", omp_get_thread_num(), i);
        }
	
	printf("\n array 1 a[i]   - ");
        for(i=0; i<n; i++) {
		printf("%d ",a[i]);
        }
      
		printf("\n  b -  %d ",b);
     
	printf("\n res array c[i] - ");
        for(i=0; i<n; i++) {
		printf("%d ",c[i]);
        }
        free(a);   free(c);
	
	return 0;
}