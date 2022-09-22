#include<omp.h>
#include<stdio.h>
int main(){
	
	#pragma omp parallel // the code in curely braces will execate an independent thread
	{
		printf("thread No. %d  Hello World\n", omp_get_thread_num());
	}
    return 0;
}
