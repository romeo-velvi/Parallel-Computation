#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h>
#define N 400 //righe
#define M 400 //colonne
#define t 10

// sommare le righe di una matrice, sommare gli elementi in riga e metterle in un array. successivamente calcolare la somma

int main(int argc, char **argv){
	int *A, *r, i,j;
	
	A=(int*)malloc((N*M)*sizeof(int));
	r=(int*)malloc((N)*sizeof(int));
	
	srand(time(NULL));

	for(i=0;i<N;i++){
		for(j=0;j<M;j++){
			A[(i*M+j)]=rand()%10;
		}
		r[i]=0;
	}

	for(i=0;i<N;i++){
		for(j=0;j<M;j++){
			printf(" %d \t",A[(i*M+j)]);
		}
		printf("\n");
	}

	double t0 = omp_get_wtime();

	#pragma omp parallel for private(i,j) reduction(+: r[:N]) num_threads(t) //collapse(2)
	for(i=0;i<N;i++){
		for(j=0;j<M;j++){
			r[i] += A[(i*M+j)];
		}
		printf("r[%d]=%d (calcolato da: %d)\n",i,r[i], omp_get_thread_num());
	}

	double t1 = omp_get_wtime();
	double tt= t1-t0;

	printf("\nPassaggio in array avvenuto in %.4f \n",tt);

	int maxval=INT_MIN;

	t0= omp_get_wtime();

	#pragma omp parallel for private(i) reduction(max: maxval)
	for(i=0;i<N;i++){
		maxval = r[i] > maxval ? r[i] : maxval;
	}
    
	t1=omp_get_wtime();
	tt=t1-t0;

	printf("IL risultato è: %d, calcolato in %.4fs\n", maxval,tt);

return 0;
}
