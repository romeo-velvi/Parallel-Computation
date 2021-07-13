#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <pthread.h>
#include <omp.h>

int main(){	

	int N; // dimensione del problema
	printf("Inserire N: ");
	scanf("%d",&N);

	float a[N]; // array di elementi da sommare (sequenziali float da 1)
	int i; // è l'invariante di ciclo di associazione e di addizioni dei threads
	printf("vettore array: ");
	for (i=0;i<N;i++){
		a[i]=i+1.0f;
		printf("%f ",a[i]);
	} printf("\n\n");

	int t; // è il numero totale dei thread disponibili
	int nloc; // è il numero di elementi da sommare per ognuno dei thread: N/ts
	int r; // è il resto della divisione: numero_degli_addendi % numero_dei_thread
	int id; // è l'identificativo privato del thread 0,1,..,N
	float sum; // per le somme parziali
	float sumtot=0; // per la somma totale
	int step; // spiazzamento nel calcolo delle somme parziali

	#pragma omp parallel private(sum,nloc,i,id,step) shared(sumtot,r)
	{
		t=omp_get_num_threads(); // si calcola il numero di thread disposti
		nloc=N/t; //si calcola quanti (interamente) saranno gli addendi per thread 
		r=N%t; // si calcola il resto della divisione
		id=omp_get_thread_num(); // si calcola l'id del thread
		/*  
			suddivisione lavoro tra i thread:
			in questo caso l'id dei thread < resro avranno addizioni in più 
		*/
		if(id<r){ //se l'id del thread è < del resto
			nloc++; // deve avere un addendo in più
			step=0;
		}else{
			step=r;
		}
		printf("numero thread: %d, di %d: numeri %d, r=%d, step=%d \n",omp_get_thread_num(),t,nloc,r,step);
		sum=0; // all'inizio la somma è = 0
		printf("sommo i valori contenuti nell'array agli indici: ");
		for(i=0;i<nloc;i++){ 
		// per ognuno degli addendi assegnati al thread si calcola la somma
			sum=sum+a[i + nloc * omp_get_thread_num() + step]; // "giochino" con indici
			printf("%d,",i + nloc * omp_get_thread_num() + step);
		} printf("\n");
		sumtot+=sum;
	} //fine direttiva
	printf("somma totale: %f\n",sumtot);
	return 0;
}

