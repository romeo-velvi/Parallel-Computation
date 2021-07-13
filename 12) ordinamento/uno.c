#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define N 50
#define t 4

void oddEvenSort(int *a){
	int sw=1, start=0, i=0, tmp;
	while(sw || start){
		sw=0;
		#pragma omp parallel for private(tmp)
		for(i=start; i<N-1;i+=2){
			if(a[i]>a[i+1]){
				tmp=a[i];
				a[i]=a[i+1];
				a[i+1]=tmp;
				sw = 1;
			}
		}
		if(start == 0)
			start = 1;
		else
			start = 0;
	}
}

void merge(int arr[], int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
 
void mergeSort(int arr[], int l, int r){
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}


void superSort(int *r){
	int i,resto,step,id,index1,index2,nloc;
	
	#pragma omp parallel private(i,resto,step,id,index1,index2) num_threads(t)
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
		
			index1= 0 + nloc * omp_get_thread_num() + step;
			index2= (nloc-1) + nloc * omp_get_thread_num() + step;
			mergeSort(r,index1,index2);
			//printf("\n Merged from %d to %d \n",index1,index2);
	}
	
	int mee=0;
	
	for(i=0;i<t;i++){
		nloc=N/t; //si calcola quanti (interamente) saranno gli addendi per thread 
		resto=N%t; // si calcola il resto della divisione
		id = omp_get_thread_num();
		if(id<resto){ //se l'id del thread è < del resto
			nloc++; // deve avere un addendo in più
			step=0;
		}else{
			step=resto;
		}
		index1 = nloc * i + step;
		index2 = (nloc-1) + nloc * i + step;
		mee=(index1+index2)/2;
		merge(r,index1,mee,index2);
	}

}



void stampavettore(int *r, char c){
	int i;
	for(i=0;i<N;i++){
		if(c=='q'){
			printf("q[%d]=%d\n",i,r[i]);
		}
		else{
			printf("r[%d]=%d\n",i,r[i]);
		}
	}
}

int main(){

	int *r, *q, *T, i,j, temp;
	
	r=(int*)malloc((N)*sizeof(int));
	q=(int*)malloc((N)*sizeof(int));
	T=(int*)malloc((N)*sizeof(int));
	
	srand(time(NULL));

	for(i=0;i<N;i++){ // array da 0 a N-1
		r[i]=rand()%N;
		//printf("r[%d]=%d\n",i,r[i]);
	}
	printf("\n\n");
	for(i=0;i<N;i++){ // array da 0 a N-1
		q[i]=rand()%N;
		//printf("q[%d]=%d\n",i,q[i]);
	}
	printf("\n\n");
    
    omp_set_num_threads(t);
    double t0,t1;

    t0 = omp_get_wtime();
    mergeSort(r,0,N-1);       
    t1 = omp_get_wtime();
    printf("TEMPO Merge sort %.6f\n",t1-t0);
    //stampavettore(r,'r');
    
	t0 = omp_get_wtime();
    superSort(q);
    t1 = omp_get_wtime();
    printf("TEMPO Super sort %.6f\n",t1-t0);
    //stampavettore(q,'q');
    
    t0 = omp_get_wtime();
    oddEvenSort(q);
   	t1 = omp_get_wtime();
    printf("TEMPO Odd-Even sort %.6f\n",t1-t0);
    //stampavettore(q,'q');
        
    
    
    return 0;
}



