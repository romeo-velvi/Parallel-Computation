#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <limits.h>
#define N 2000 //righe
#define t 3 //thread

int main(int argc, char **argv){
	int *r, i, resto, nloc, trova;
	
	r=(int*)malloc((N)*sizeof(int));
	
	srand(time(NULL));

	for(i=0;i<N;i++){ // array da 0 a N-1
		r[i]=i+1;
		printf("r[%d]=%d\n",i,r[i]);
	}
	
    printf("\nInserire il numero da cercare: ");
    scanf("%d", &trova);
	
	int foundat=-1, index=-1, step, id;


	double t0 = omp_get_wtime();

	#pragma omp parallel private(i,resto,step,id,index) shared(trova,foundat) num_threads(t)
	{
		nloc=N/t; //si calcola quanti (interamente) saranno gli addendi per thread 
		resto=N%t; // si calcola il resto della divisione
		id = omp_get_thread_num();
		if(id<resto){ //se l'id del thread è < del resto
			nloc++; // deve avere un addendo in più
			step=0;
		}else{
			step=resto;
		}
		
		for(i=0;i<nloc;i++){ 

			index= i + nloc * omp_get_thread_num() + step;
			if(r[index]==trova){
				printf("\nprocesso (%d) ha trovato l'occorrenza nella posizione [%d] \n",id,index);
				
				foundat=index;
					
				#pragma omp cancel parallel
			}
		}
	}
	

	double t1 = omp_get_wtime();
	double tt= t1-t0;

	printf("\nElemento trovato in %.6f secondi \n",tt);

	if(foundat==-1){
		printf("\nelemento non trovato\n");
	}
	else{
		printf("indice trovato a [%d], valore da trovare = %d  valore trovato = (%d)\n", foundat,trova,r[foundat]);
	}
return 0;
}
