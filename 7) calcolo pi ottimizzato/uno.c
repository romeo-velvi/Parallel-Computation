#include <stdio.h>
#include <omp.h>
#define N 100000000
int main(int argc, char **argv)
{
    long int i, n = N;
    double x, dx, f, sum, pi;
    printf("number di intervalli: %ld\n", n);
    sum = 0.0;
    double start,end;

    //grandezza di h
    dx = 1.0 / (double) n;
    start=omp_get_wtime();
    //ho dovuto togliere sum tra i parametri shared per la reduction
    #pragma omp parallel for private(x,f,i) reduction(+:sum)
    for (i = 1; i <= n; i++)
    {
         x = dx * ((double) (i - 0.5));
         f = 4.0 / (1.0 + x*x);
         //commento #pragma omp critical 
         //poichè la reduction esegue sequenzialmente le variabili date come argomento
         sum = sum + f;
    }
    
    end=omp_get_wtime();//scansisce il tempo di esecuzione
    printf("Tempo: %f\n",end-start);

     pi = dx*sum;
     printf("PI %.7f\n", pi);

return 0;
}
