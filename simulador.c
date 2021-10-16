/* ------------------
 *  mainf.c
 * ----------------------------------------------
 *  Parte 1 de práctica de la asignatura SESO
 *	Autor: Borja Moralejo Tobajas
 *	Fecha: 1 de Octubre de 2021s
 * ---------------------------------------------- */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "defines.h"
#include "estructuras.h"
#include "queue.h"
#include "test.h"
#include <getopt.h>
#define PRINTF_EN 1

mempool_pcb_t memPCBs;
lkdList_int_t *cola_idx;
queue_pcb_t q_pcb;
machine_t machine;


// Parámetros default
int param_nElementosCola;
int param_nPool;
int n_cpu;
int n_core;
int n_thread;

void procesarParametros(int argc, char *argv[]);

int main(int argc, char *argv[]){

	// Poniendo los valores default de las variables
	param_nPool = DEFAULT_POOLSIZE;
	param_nElementosCola = DEFAULT_QUEUESIZE; 
	n_cpu = DEFAULT_CPU;
	n_core = DEFAULT_CORE;
	n_thread = DEFAULT_THREAD;

	// Tratar parámetros
	procesarParametros(argc, argv);

   /*---------------------------------*
	- Inicialización de los elementos -
	*---------------------------------*/

	// 	Inicializar el almacenador de PBS
	inicializar_estructura(param_nPool);
	// 	Inicializar el indexador de PBS
	inicializar_linkedList_int(&memPCBs.cola_idx, param_nPool);
	// 	Inicializar colas del dispatcher
	inicializar_queue_pcb(&q_pcb, param_nElementosCola);
	
	//test(param_nElementosCola);
	
	// Inicializar elementos de sincronización

	// Lanzar hilos que implementaran los distintos subsistemas
	
	// Realizar la comunicación utilizando memoria compartida y los elementos de sincronización vistos en clase.



	// Liberar recursos asignados
	free(q_pcb.malloc);
	free(memPCBs.cola_idx.malloc);	
	free(memPCBs.malloc);
}

void getInt(char * _numero, char letra, int * var){
	char *fin;
	long value = strtol(_numero, &fin, 10); 
	if (fin == _numero || *fin != '\0' || errno == ERANGE)
	{
		fprintf(stderr, "Error, dato introducido no es un número: %s\n",_numero);
		return;
	}
	if(value <= 0)
	{
		fprintf(stderr, "%c: No puede ser negativo o cero: %d\n",letra, (int) value );
		return;
	}	
	*var = (int) value;
}
void procesarParametros(int argc, char *argv[]){ 
 	int opt, longindex;
	opterr = 0;
 	struct option long_options[] = {
	{"poolsize", required_argument, 0, 'p' },
	{"queuesize", required_argument, 0, 'q' },
	{"n_cpu", required_argument, 0, 'c' },
	{"n_core", required_argument, 0, 'k' },
	{"n_thread", required_argument, 0, 't' },
	{"help", no_argument, 0, 'h' },
	{0, 0, 0, 0 }
	};


	longindex =0;
	while ((opt = getopt_long(argc, argv,"hq:p:c:k:t:", long_options, &longindex )) != -1) 
	{

		switch(opt) 
		{
		case 'p': /* poolsize */
			getInt(optarg , 'p', &param_nPool);
			break;
		case 'q': /* queuesize */
			getInt(optarg, 'q', &param_nElementosCola);
			break;
		case 'c': /* n_cpu */
			getInt(optarg, 'c', &n_cpu);
			break;
		case 'k': /* n_core */
			getInt(optarg, 'k', &n_core);
			break;
		case 't': /* n_thread */
			getInt(optarg, 't', &n_thread);
			break;
		case 'h': /* -h or --help */
		case '?':
			printf ("Uso %s [OPTIONS]\n", argv[0]);
			printf (" -c, --n_cpu=n\t\t"
			"Número de cpus: %d\n", DEFAULT_CPU);
			printf (" -k, --n_core=n\t\t"
			"Número de cores: %d\n", DEFAULT_CORE);
			printf (" -t, --n_thread=n\t"
			"Número de threads: %d\n", DEFAULT_THREAD);
			printf (" -p, --poolsize=n\t"
			"Tamaño de pool: %d\n", DEFAULT_POOLSIZE);
			printf (" -q, --queuesize=n\t"
			"Tamaño de cola: %d\n", DEFAULT_QUEUESIZE);
			printf (" -h, --help\t\t"
			"Ayuda\n");
			exit (2);
		default:
			fprintf(stderr, "Argumento desconocido, error\n");
			exit(4);
		} // switch
	} // while
} // procesarParam