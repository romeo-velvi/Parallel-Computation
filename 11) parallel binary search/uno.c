#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int binarySearch(int left, int right,int t,int token, int *a);

int main()
{

    int n, i, token, t = 3, result, *a;

    printf("Inserire la dimensione del vettore: ");
    scanf("%d", &n);

    a = (int *)malloc(n*sizeof(int));

    for(i = 0; i < n; i++)
    {
        a[i] = i;
        printf("[%d]\t", a[i]);
    }

    printf("\nInserire il numero da cercare: ");
    scanf("%d", &token);

     omp_set_num_threads(t);

    double t0 = omp_get_wtime();
    result = binarySearch(0, n - 1, t, token, a);
    double t1 = omp_get_wtime();

    if (result < 0)
        printf("The number not is in the vector.\n");
    else
        printf("The number is in the position: %d\n", result);

    printf("TEMPO %.6f\n",t1-t0);
    return 0;
}



int binarySearch(int left, int right,int t, int token, int *a)
{   
    if(token<a[left] || token>a[right])
        return -1;

    int result;
    int leftThread,rightThread,id;

    int mid = (right - left + 1)/2; //Sottoporzione del vettore.


    if (mid == 0)  //Se il vettore è composto da un solo elemento.
        return (a[left] != token) ?  -1 :  left;

    if(a[mid] == token)
        return a[mid];

    omp_set_nested(1);


    #pragma omp parallel shared(a, token, left, right, mid, result) private(leftThread,rightThread,id)
    {
         id = omp_get_thread_num();
         leftThread = left + id * mid;
         rightThread = (id == t-1) ? right : leftThread + mid - 1;

        if(a[leftThread] <= token && a[rightThread] >= token)
             result = binarySearch(leftThread, rightThread,t,token, a);
        
    }
}
