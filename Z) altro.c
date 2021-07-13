#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void ParallelRandomMatrixGenerator(int *matrix,int n_righe,int n_colonne,int number_threads)
{
 int j_start,size_porzione;
 int thread_id;
 srand(time(NULL));
 #pragma omp parallel num_threads(number_threads),shared(matrix,n_righe,n_colonne),private(size_porzione,thread_id,j_start)
 {
  size_porzione = n_colonne/number_threads;                                 //i blocchi gestiti da ogni thread sono blocchi di colonne
  thread_id = omp_get_thread_num();
  if(thread_id == 0)
  {
   printf("Numero di threads in esecuzione per generare la matrice: %d\n",omp_get_num_threads());
  }
  j_start = thread_id * size_porzione;                                       //se ogni thread si occupa di una porzione di lunghezza size_porzione,se ne deduce che un thread si occuperà di una porzione che parte da un indice multiplo del size di tale porzione
  if(thread_id == number_threads - 1)
  {
   size_porzione = size_porzione + (n_colonne%number_threads);                //l'ultimo thread,eventualmente,si occuperà di più elementi se il numero di colonne non è perfettamente divisibile per il numero di threads
  }
  for(int i=0;i<n_righe;i++)
  {
   for(int j=j_start;j<j_start+size_porzione;j++)
   {
    *(matrix + i*n_colonne + j) = 1+rand()%9;
   }
  }
 }
}

void printMatrix(int *matrix,int n_righe,int n_colonne)
{
 for(int i=0;i<n_righe;i++)
 {
  for(int j=0;j<n_colonne;j++)
  {
   printf("%d ",*(matrix + i*n_colonne + j));
  }
  printf("\n");
 }
 printf("\n");
}

void ParallelRandomVectorGenerator(int *vector,int length,int number_threads,int superior_limit)
{
 int i_start,size_porzione;
 int thread_id;
 srand(time(NULL));
 #pragma omp parallel num_threads(number_threads),shared(vector,length),private(size_porzione,i_start,thread_id)
 {
  thread_id = omp_get_thread_num();
  if(thread_id == 0)
  {
   printf("Numero di threads in esecuzione per generare il vettore: %d\n",omp_get_num_threads());
  }
  size_porzione = length/number_threads;
  i_start = thread_id * size_porzione;                                                   //prima si assegna l'indice corretto di inizio porzione...
  if(thread_id == number_threads - 1)
  {
   size_porzione = size_porzione + length%number_threads;                            //...poi si aggiusta,eventualmente,la lunghezza della porzione gestita dall'ultimo thread
  }
  /*printf("i_start del thread %d: %d\n",thread_id,i_start);*/
  for(int i=i_start;i<i_start+size_porzione;i++)
  {
   *(vector + i) = 1 + rand()%superior_limit;
  }
 }
}

void printVector(int *vector,int length)
{
 for(int i=0;i<length;i++)
 {
  printf("%d ",*(vector + i));
 }
 printf("\n");
}

/*
Implementazione della prima strategia di parallelizzazione del prodotto matrice-vettore.
La matrice e' divisa in blocchi di righe e il vettore colonna è condiviso per intero tra i thread che effettuano il calcolo.
*/
void ParallelResolveLinearSystemRowBlocks(int *matrice_a,int n_righe,int n_colonne,int *vettore_b,int *vettore_risultato,int number_threads)
{
 int i_start,righe_blocco;
 int thread_id;
 #pragma omp parallel num_threads(number_threads),firstprivate(vettore_b),shared(matrice_a,vettore_risultato,n_righe,n_colonne),private(thread_id,righe_blocco,i_start)
 {
  thread_id = omp_get_thread_num();
  righe_blocco = n_righe/number_threads;                                 //...ogni thread si occupera' di un blocco di n_righe/number_threads righe...
  if(thread_id == 0)
  {
   printf("Numero di threads in esecuzione per risolvere il sistema lineare: %d\n",omp_get_num_threads());
  }
  if((n_righe%number_threads) == 0)                                         //se il numero di righe e' perfettamente suddivisibile tra i threads,nessun problema...
  {
   i_start = thread_id * righe_blocco;                                   //...tale blocco partira' dalla riga di indice i_start.
  }
  else                                                                     //ma se non sono perfettamente suddivisibili...
  {
   if(thread_id < n_righe%number_threads)                               //...per tutti i thread di identificativo strettamente minore di n_righe%number_threads...
   {
    righe_blocco = righe_blocco + 1;                                  //...assegnamo al thread una riga in più su cui ciclare...
    i_start = thread_id * righe_blocco;                               //...e correggiamo il suo indice di partenza
   }
   else
   {
    i_start = (thread_id * righe_blocco) + n_righe%number_threads;                       //ora le porzioni gestite dai thread con [TID > n_righe%number_threads] risultano shiftate di n_righe%number_threads posizioni
   }
  }
  /*
  printf("i_start: %d\n",i_start);
  printf("righe blocco: %d\n",righe_blocco);
  */
  for(int i=i_start;i<i_start+righe_blocco;i++)
  {
   for(int j=0;j<n_colonne;j++)
   {
    *(vettore_risultato + i) = *(vettore_risultato + i) + (*(matrice_a + i*n_colonne + j)*(*(vettore_b + j)));
   }
   /*printf("risultato[%d] = %d\n",i,*(vettore_risultato + i));*/
  }
 }
}

/*
Implementazione della prima strategia del prodotto matrice-vettore usando il costrutto parallel for.
*/
void ParallelResolveLinearSystemRowBlocksForConstruct(int *matrice_a,int n_righe,int n_colonne,int *vettore_b,int *vettore_risultato,int number_threads)
{
 #pragma omp parallel num_threads(number_threads),shared(matrice_a,n_righe,n_colonne,vettore_b,vettore_risultato)
 {
  #pragma omp for
  for(int i=0;i<n_righe;i++)
  {
   for(int j=0;j<n_colonne;j++)
   {
    *(vettore_risultato + i) = *(vettore_risultato + i) + (*(matrice_a + i*n_colonne + j)*(*(vettore_b + j)));
   }
  }
 }
}

/*
Implementazione della seconda strategia del prodotto matrice-vettore.
La matrice e' divisa in blocchi di colonne e il vettore colonna è condiviso tra i thread del team,ma per la logica dell'algoritmo,
ogni thread accede a una porzione diversa del vettore colonna.
*/
void ParallelResolveLinearSystemColumnBlocks(int *matrice_a,int n_righe,int n_colonne,int *vettore_b,int *vettore_risultato,int number_threads)
{
 int thread_id;
 int j_start,columns_per_block;
 int *local_result;
 int cell_result = 0;                                                        //conterrà il risultato di una specifica cella del vettore intermedio risultante
 #pragma omp parallel num_threads(number_threads),private(thread_id,j_start,local_result,columns_per_block),firstprivate(cell_result),shared(matrice_a,vettore_b,n_righe,n_colonne,vettore_risultato)
 {
  local_result = calloc(n_righe,sizeof(int));                                    //ATTENZIONE: la calloc deve avvenire all'interno,altrimenti l'indirizzo di memoria allocato sarà sempre lo stesso,anche se la variabile sarà firstprivate
  thread_id = omp_get_thread_num();
  columns_per_block = n_colonne/number_threads;
  if((n_colonne%number_threads) == 0)                            //se il numero di colonne e' perfettamente suddivisibile per il numero di threads,non c'e' problema...
  {
   j_start = thread_id * columns_per_block;                      //...l'indice di inizio porzione e' ottenibile semplicemente facendo una moltiplicazione di thread_id e columns_per_block
  }
  else                                                          //...ma se il numero di colonne non e' perfettamente suddivisibile per il numero dei threads...
  {
   if(thread_id < n_colonne%number_threads)                     //...a tutti i thread con id strettamente minore di n_colonne%number_threads...
   {
    columns_per_block = columns_per_block + 1;                    //...si assegna una colonna in più su cui ciclare...
    j_start = thread_id * columns_per_block;                      //...e si ricalcola l'indice corretto di inizio porzione
   }
   else
   {
    j_start = (thread_id * columns_per_block) + n_colonne%number_threads;                   //i thread con id superiore di n_colonne%number_threads il loro indice di inizio porzione shiftato di n_colonne%number_threads colonne
   }
  }
  for(int i=0;i<n_righe;i++)
  {
   for(int j=j_start;j<j_start+columns_per_block;j++)
   {
    cell_result = cell_result + (*(matrice_a + i*n_colonne + j) * (*(vettore_b + j)));
   }
   *(local_result + i) = cell_result;
   cell_result = 0;                                                                                   //azzeramento della variabile per poterla utilizzare in maniera corretta con un altra cella
  }
  #pragma omp barrier
  #pragma omp critical
  {
   for(int i=0;i<n_righe;i++)
   {
    //printf("i: %d\t|TID: %d\t|local_result[i]: %d\n",i,thread_id,*(local_result + i));
    *(vettore_risultato + i) = *(vettore_risultato + i) + *(local_result + i);
   }
  }
 }
}

void ParallelScalarVectorsProduct(int *a,int *b,int size_vectors,int number_threads,int *c)
{
 int i_start,size_porzione;
 int thread_id;
 #pragma omp parallel num_threads(number_threads),shared(a,b,c,size_vectors),private(i_start,size_porzione,thread_id)
 {
  thread_id = omp_get_thread_num();
  size_porzione = size_vectors/number_threads;
  if(thread_id == 0)
  {
   printf("Numero di threads in esecuzione per effettuare il prodotto scalare tra vettori: %d\n",omp_get_num_threads());
  }
  if(thread_id < size_vectors%number_threads)
  {
   size_porzione = size_porzione + 1;
   i_start = thread_id * size_porzione;
  }
  else
  {
   i_start = (thread_id * size_porzione) + (size_vectors%number_threads);
  }
  for(int i=i_start;i<i_start+size_porzione;i++)
  {
   *(c + i) = (*(a + i)) * (*(b + i));
   /*printf("c[%d] = %d\n",i,*(c + i));*/
  }
 }
}

void ParallelScalarVectorsSum(int *a,int *b,int size_vectors,int number_threads,int *c)
{
 int i_start,size_porzione;
 int thread_id;
 #pragma omp parallel num_threads(number_threads),shared(a,b,c,size_vectors),private(i_start,size_porzione,thread_id)
 {
  thread_id = omp_get_thread_num();
  size_porzione = size_vectors/number_threads;
  i_start = thread_id * size_porzione;
  if(thread_id == 0)
  {
   printf("Numero di threads in esecuzione per effettuare la somma dei vettori: %d",omp_get_num_threads());
  }
  if(thread_id == number_threads - 1)
  {
   if((size_vectors%number_threads) != 0)
   {
    size_porzione = size_porzione + (size_vectors%number_threads);
   }
  }
  for(int i=i_start;i<i_start+size_porzione;i++)
  {
   *(c + i) = (*(a + i)) + (*(b + i));
  }
 }
}

void ParallelVectorFlip(int *a,int *b,int size_vectors,int number_threads)
{
 int size_porzione,i_start;
 int thread_id;
 #pragma omp parallel num_threads(number_threads),shared(a,b,size_vectors),private(size_porzione,i_start,thread_id)
 {
  thread_id = omp_get_thread_num();
  size_porzione = size_vectors/number_threads;
  i_start = thread_id * size_porzione;
  if((thread_id == number_threads-1) && ((size_vectors%number_threads) != 0))
  {
   size_porzione = size_porzione + (size_vectors%number_threads);
  }
  if(thread_id == 0)
  {
   printf("Numero di threads attivi per effettuare lo swap: %d\n",omp_get_num_threads());
  }
  for(int i=i_start;i<i_start+size_porzione;i++)
  {
   /*
   printf("i del thread %d: %d\n",thread_id,i);
   printf("corrispondente nell'array swappato: %d\n",size_vectors-1-i);
   printf("-------------------------------------------------\n");
   */
   *(b + size_vectors - 1 - i) = *(a + i);                                     //l'indice swappato di una porzione è comunque calcolato rispetto alla dimensione complessiva dell'array NON rispetto alla dimensione della porzione di appartenenza dell'elemento
  }
 }
}

void ParallelVectorCopy(int *a,int *b,int size_vectors,int number_threads)
{
 int size_porzione,i_start;
 int thread_id;
 #pragma omp parallel num_threads(number_threads),shared(a,b,size_vectors),private(size_porzione,i_start,thread_id)
 {
  thread_id = omp_get_thread_num();
  size_porzione = size_vectors/number_threads;
  i_start = thread_id * size_porzione;
  if((thread_id == number_threads-1) && ((size_vectors%number_threads) != 0))
  {
   size_porzione = size_porzione + (size_vectors%number_threads);
  }
  if(thread_id == 0)
  {
   printf("Numero di threads attivi per effettuare la copia: %d\n",omp_get_num_threads());
  }
  for(int i=i_start;i<i_start+size_porzione;i++)
  {
   //printf("i del thread %d: %d\n",thread_id,i);
   *(b + i) = *(a + i);
  }
 }
}

void ParallelMatricesProduct(int *matrice_a,int rows_a,int columns_a,int *matrice_b,int rows_b,int columns_b,int number_threads,int *matrice_c)
{
 int thread_id;
 int i_startA;
 int rowsA_per_block;
 int cell_result;
 int j_b;
 #pragma omp parallel num_threads(number_threads),private(thread_id,j_b,i_startA,rowsA_per_block,cell_result),shared(matrice_a,rows_a,columns_a,matrice_b,rows_b,columns_b,matrice_c)
 {
  cell_result = 0;
  thread_id = omp_get_thread_num();
  rowsA_per_block = rows_a/number_threads;
  i_startA = thread_id * rowsA_per_block;
  if((rows_a%number_threads != 0) || (columns_b%number_threads != 0))                    //se le righe della matrice A e le colonne della matrice B non sono perfettamente suddivisibili tra i threads del team...
  {
   if(thread_id < rows_a%number_threads)                                //...i thread con id strettamente minore delle righe restanti della matrice A...
   {
    rowsA_per_block = rowsA_per_block + 1;                            //...avranno una riga in più della matrice A su cui ciclare...
    i_startA = thread_id * rowsA_per_block;
   }
   else
   {
    i_startA = i_startA + (rows_a%number_threads);                      //...e i thread con id maggiore o uguale al resto sposteranno il loro indice di inizio porzione di (rows_A%number_threads) posizioni
   }
   if(thread_id == 0)
   {
    printf("Threads in esecuzione per generare il prodotto tra le matrici: %d\n",omp_get_num_threads());
   }
   for(int i_a=i_startA;i_a<i_startA+rowsA_per_block;i_a++)
   {
    for(int j_b=0;j_b<columns_b;j_b++)
    {
     for(int j_a=0;j_a<columns_a;j_a++)
     {
      cell_result = cell_result + (*(matrice_a + i_a*columns_a + j_a) * (*(matrice_b + j_a*columns_b + j_b)));
     }
     *(matrice_c +  i_a*columns_b + j_b) = cell_result;
     cell_result = 0;
    }
   }
  }
 }
}

