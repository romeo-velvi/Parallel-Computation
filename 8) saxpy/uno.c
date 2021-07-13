#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define N 3
#define M 4

int main(int argc, char const *argv[])
{
	int *A,*a,*b,alfa,beta,*r;
	int i,j;
	
	A = (int *) malloc((M*N)*sizeof(int));
	a = (int *) malloc(M*sizeof(int));
	b = (int *) malloc(N*sizeof(int));
	r = (int *) malloc(M*sizeof(int));

	alfa=6;
	beta=7;

	srand(time(NULL));

	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			*(A+i*N+j) = rand()%10;
			b[j] = j+3;
		}
		a[i] = i+1;
		r[i]=0;
	}

	#pragma omp parallel for private(i,j) reduction(+: r[:M])
	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			r[i] += alfa*A[i*N+j] * b[j];
		}
		r[i] += beta * a[i];
	}

	long p=1;
	#pragma omp parallel for private(i) reduction(*:p)
		for(i=0;i<M;i++)
			p *= r[i];
	
	printf("RISULTATI PARALLELO\n");
	for(i=0;i<M;i++)
		printf("%d\n",r[i]);
	printf("%ld\n",p);

	printf("\n");

printf("RISULTATI SEQUENZIALE\n");
p=1;
for(i=0;i<M;i++){
	r[i] = 0;
		for(j=0;j<N;j++){
			r[i] += alfa*A[i*N+j] * b[j];
		}
		r[i] += beta * a[i];
	}

	for(i=0;i<M;i++)
			p *= r[i];

	for(i=0;i<M;i++)
		printf("%d\n",r[i]);
	printf("%ld\n",p);
	return 0;
}
