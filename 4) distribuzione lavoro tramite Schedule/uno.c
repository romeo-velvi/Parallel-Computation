#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 16
#define CHUNK 10
int main( ){
	int i;
	int n_threads;
	printf("quanti thread inserire: ");
	scanf("%d",&n_threads);
	double start=omp_get_wtime();
	#pragma omp parallel for private(i) schedule(static) num_threads(n_threads)
	//#pragma omp parallel for private(i) schedule(dynamic,CHUNK) num_threads(n_threads)
	//#pragma omp parallel for private(i) schedule(guided) num_threads(n_threads)
	for (i=0; i<N; i++)
	{
		//aspetta i secondi
		sleep(i);
		printf("Il thread %d ha completato iterazione %d.\n", omp_get_thread_num( ) , i);
	}
	double end=omp_get_wtime();//scansisce il tempo di esecuzione
    printf("Tempo: %f\n",end-start);
	printf("Tutti I thread hanno terminato!\n");
	return 0;
}
