#include<stdio.h>
#include<omp.h>
 
 int main(){
 	
 	long long sum = 0;
 	
 	double inTime = omp_get_wtime();
 	
 	
 	int i;
 	for(i=1;i<=100000000;i++){
 		sum += (i*i);
	 }
	 
	 
	 double outTime = omp_get_wtime();
	 
	 double expcTime = outTime - inTime;
	 
	 printf("Time Required for Execution in Serial : %f\n",expcTime);
	 printf("Answer is : %lld \n",sum);
 	
 	return 0;
 }
