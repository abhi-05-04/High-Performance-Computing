#include<omp.h>
#include<stdio.h>
static int sum =0;
int main()
{
    #pragma omp parallel for reduction(+ : sum)
    for(int i=1; i<=100;i++)
    {
        // printf("thread No. %d Number : %d Square : %d\n", omp_get_thread_num(), i, i * i);  
        sum+=i*i;      
	}
    printf("Sum of first 100 numbers is %d\n\n",sum);
    return 0;
}
