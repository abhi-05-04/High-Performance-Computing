#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    clock_t begin = clock();
    int n = 200, i ,j;  
    #pragma omp parallel 
    {
        // #pragma omp for nowait
        for(i = 0; i < 10; i++)
        {
            printf("first for %d\n",i);
        }

        // #pragma omp for nowait
        for(j = 0; j < 10; j++)
        {
            printf("second for %d\n",j);
        }

    }

    printf("Outside for ==\n ");

    clock_t end = clock();
    double time_execution = (double)(end-begin)/CLOCKS_PER_SEC;
    printf("\nTime for execution: %lf\n", time_execution);
    return 0;
}