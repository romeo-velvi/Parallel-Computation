#include <omp.h>
#include <stdio.h>
int main()
{
	int id_thread;
	int var_cond=0;
	int var_priv=0;
	#pragma omp parallel private(id_thread,var_priv), shared (var_cond)
	{
		id_thread = omp_get_thread_num();
		var_cond+=1;
		var_priv+=1;
		printf("thread %d, variabile condivisa: %d  variabile privata: %d\n", id_thread, var_cond, var_priv);
	} 
	return 0;
}
