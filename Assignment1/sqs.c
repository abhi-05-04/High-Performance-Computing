//sequencial
//finding sum of squares of 1 to 100
#include<omp.h>
#include<stdio.h>
#include <sys/time.h>


int main(){
	
	// int id;
	int sum=0;
	int square=0;
	int pointer=1;
// 	struct timeval  tv1, tv2;
// gettimeofday(&tv1, NULL);
	while(pointer <101){
		square = pointer*pointer;
		sum+=square;
		pointer++;			
	}
		
// 	printf("\nSum of squares till 100 is : %d  \n",sum);
// 	gettimeofday(&tv2, NULL);

// printf ("Total time = %f seconds\n",
//          (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//          (double) (tv2.tv_sec - tv1.tv_sec));
	return 0;
}