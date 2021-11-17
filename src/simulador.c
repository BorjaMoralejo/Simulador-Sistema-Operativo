/* ------------------
 *  mainf.c
 * ----------------------------------------------
 *  Parte 1 de práctica de la asignatura SESO
 *	Autor: Borja Moralejo Tobajas
 *	Fecha: 1 de Octubre de 2021
 * ---------------------------------------------- */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "defines.h"
#include "estructuras.h"
#include "queue.h"
#include "clock.h"
#include "timer.h"
#include "scheduler.h"
#include "pgenerator.h"

#define PRINTF_EN 1

mempool_pcb_t memPCBs;
lkdList_int_t *cola_idx;
queue_pcb_t q_pcb;
param_init_t paramStruct;
matrix3_t matrix3;
machine_t machine;

// Parámetros default
void procesarParametros(int argc, char *argv[], param_init_t *_params);

int main(int argc, char *argv[]){
	// Declaraciones de variables
	pthread_t* arrThreads;
	void *arrParametros;
	int i;
	matrix3_t m3;
	matrix3 = m3;
	param_init_t pit;
	paramStruct = pit;
	machine_t mach;
	machine = mach;

	// Declaraciones de variables de elementos
	sched_disp_data_t * sched_arr;
	int n_slaves;
	// Poniendo los valores default de las variables
	paramStruct.nPool = DEFAULT_POOLSIZE;
	paramStruct.nElementosCola = DEFAULT_QUEUESIZE; 
	paramStruct.n_cpu = DEFAULT_CPU;
	paramStruct.n_core = DEFAULT_CORE;
	paramStruct.n_thread = DEFAULT_THREAD;
	paramStruct.clock_manual = DEFAULT_CLOCK_MANUAL;
	paramStruct.clock_retardado = DEFAULT_CLOCK_RETARDADO;
	paramStruct.freq_clock_ms = DEFAULT_FREQ_CLOCK_MS;
	paramStruct.freq_disps_sched = DEFAULT_FREQ_DISP_SCHED;
	paramStruct.freq_pgen = DEFAULT_FREQ_PGEN;
	paramStruct.ttl_base = DEFAULT_TTL_BASE;
	paramStruct.ttl_max = DEFAULT_TTL_MAX;

	paramStruct.blocked_list_size = DEFAULT_BLOCKED_LIST_SIZE;
	paramStruct.block_chance = DEFAULT_BLOCK_CHANCE;
	paramStruct.max_blocked_time = DEFAULT_MAX_BLOCKED_TIME;
	paramStruct.random_priority = DEFAULT_RANDOM_PRIORITY;
	paramStruct.random_affinity = DEFAULT_RANDOM_AFFINITY;
	paramStruct.freq_reschedule = DEFAULT_FREQ_DISP_SCHED;
	paramStruct.quantum_per_prio = DEFAULT_Q_PER_PRIORITY;
	paramStruct.max_prio = DEFAULT_MAX_PRIO;
	paramStruct.pcb_generated = DEFAULT_PCB_GENERATED;

	paramStruct.politica_scheduler_master = DEFAULT_MASTER_SCHEDULER_BEHAVIOUR;
	paramStruct.politica_scheduler_slave = 0;

	// Tratar parámetros
	procesarParametros(argc, argv, &paramStruct);
	
   /*---------------------------------*
	| Inicialización de los elementos |
	*---------------------------------*/

	// 	Almacenador de PBS
	inicializar_estructura(paramStruct.nPool);
	// 	Indexador de PBS
	inicializar_linkedList_int(&memPCBs.cola_idx, paramStruct.nPool);
	// 	Cola del dispatcher
	inicializar_queue_pcb(&q_pcb, paramStruct.nElementosCola);
	// Máquina del simulador
	init_machine(&paramStruct, &matrix3, &machine);

	// Inicializar elementos de sincronización
	init_clock();
	init_scheduler(&sched_arr);
	init_pgen();

	/*------------------------*
	 |	Lanzamiento de hilos  |
	 *------------------------*
	 Resumen:
	 Lanzar hilos que implementaran los distintos subsistemas
	 Realizar la comunicación utilizando memoria compartida y los elementos de sincronización vistos en clase.
	*/
		 
	// Iniciando hilos
	printf("Iniciando hilos...\n");
	n_slaves = paramStruct.n_cpu*paramStruct.n_core;
	arrThreads = malloc(sizeof(pthread_t)*(NTHREADS + n_slaves));
	// ----------CLOCK------------
	pthread_create(&arrThreads[CLOCK_TH], NULL, &start_clock, NULL);

	// ----------TIMERS------------
	// Por ahora el timer recibe dos parámetros, el id y la funcion dedicada

	// Timer de dispatcher
	arrParametros = malloc(sizeof(short int)*2); // falta liberar memoria
	((short *)arrParametros)[0] = 0;	 
	((short *)arrParametros)[1] = DISPATCHER_SCHEDULER_FUNC;
	pthread_create(&arrThreads[TIMER0_TH], NULL, &start_timer, arrParametros);

	// Timer de pgenerator
	arrParametros = malloc(sizeof(short int)*2); // falta liberar memoria
	((short *)arrParametros)[0] = 0;	 
	((short *)arrParametros)[1] = PGENERATOR_FUNC;
	pthread_create(&arrThreads[TIMER1_TH], NULL, &start_timer, arrParametros);


	// ----------SCHEDULER MAESTRO------------
	pthread_create(&arrThreads[SCHEDULER_TH], NULL, &start_schedule_master, sched_arr);
	// ----------SCHEDULERS ESCLAVOS------------

	for ( i = 0; i < n_slaves; i++ )
		pthread_create(&arrThreads[NTHREADS + i], NULL, &start_schedule_slave, &sched_arr[i]);

	
	// ----------PGenerator------------
	pthread_create(&arrThreads[PGEN_TH], NULL, &start_pgenerator, NULL);

	printf("Hilos iniciados\n");

	// Esperando a que terminen y recogerlos
	// Por ahora no sirve de nada
	for(i = 0; i < NTHREADS + n_slaves; i++)
	{
		pthread_join(arrThreads[i], NULL);
	}
	
	// TODO: forma de que acabe el simulador de forma controlada
}

// Función auxiliar para asignar el valor del argumento al entero del struct
// y tener un control de errores muy simple
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

// Procesar los argumentos y los mete en el struct de parametros
void procesarParametros(int argc, char *argv[], param_init_t * _params){ 
 	int opt, longindex;
	int buff;
	opterr = 0;
 	struct option long_options[] = {
	{"poolsize", required_argument, 0, 'p' },
	{"queuesize", required_argument, 0, 'q' },
	{"clock_manual", no_argument, 0, 'm' },
	{"clock_retardado", no_argument, 0, 'r' },
	{"freq_clock_ms", required_argument, 0, 'l' },
	{"freq_disps_sched", required_argument, 0, 'd' },
	{"freq_pgenerator", required_argument, 0, 'g' },
	{"ttl_base", required_argument, 0, 'b'},
	{"ttl_max", required_argument,0 , 'x'},
	{"n_cpu", required_argument, 0, 'c' },
	{"n_core", required_argument, 0, 'k' },
	{"n_thread", required_argument, 0, 't' },
	{"help", no_argument, 0, 'h' },
	{0, 0, 0, 0 }
	};


	longindex =0;
	while ((opt = getopt_long(argc, argv,"hmq:p:c:k:t:rl:d:g:b:x:", long_options, &longindex )) != -1) 
	{

		switch(opt) 
		{
		case 'r': /* clock retardado */
			if(_params->clock_manual){
				printf("Negando argumento de clock manual\n");
				_params->clock_manual = 0;
			}
				_params->clock_retardado = 1;
			break;
		case 'd': /* freq_disps_sched */
			getInt(optarg , 'd', &_params->freq_disps_sched);
			break;
		case 'g': /* freq_pgenerator */
			getInt(optarg , 'g', &_params->freq_pgen);
			break;
		case 'b': /* ttl_base */
			getInt(optarg , 'b', &_params->ttl_base);

			break;
		case 'x': /* ttl_max */
			getInt(optarg , 'x', &_params->ttl_max);
			break;
		case 'm': /* clock_manual */	
			_params->clock_manual = 1;
			break;
		case 'l': /* freq_clock_ms */
			getInt(optarg , 'l', &_params->freq_clock_ms);
			break;
		case 'p': /* poolsize */
			getInt(optarg , 'p', &_params->nPool);
			break;
		case 'q': /* queuesize */
			getInt(optarg, 'q', &_params->nElementosCola);
			break;
		case 'c': /* n_cpu */
			getInt(optarg, 'c', &_params->n_cpu);
			break;
		case 'k': /* n_core */
			getInt(optarg, 'k', &_params->n_core);
			break;
		case 't': /* n_thread */
			getInt(optarg, 't', &_params->n_thread);
			break;
		case 'h': /* -h or --help */
		case '?':
			printf ("Uso %s [OPTIONS]\n", argv[0]);
			printf (" -m, --clock_manual\t\t"
			"flag Clock manual: %d\n", DEFAULT_CLOCK_MANUAL);
			printf (" -r, --clock_retardado\t\t"
			"flag Clock retardado: %d\n", DEFAULT_CLOCK_RETARDADO);
			printf (" -l, --freq_clock_ms=n\t\t"
			"Frecuencia clock en ms: %d\n", DEFAULT_FREQ_CLOCK_MS);
			printf (" -d, --freq_disps_sched=n\t"
			"Frecuencia dispacher/scheduler: %d\n", DEFAULT_FREQ_DISP_SCHED);
			printf (" -g, --freq_pgen=n\t\t"
			"Frecuencia pgenerator: %d\n", DEFAULT_FREQ_PGEN);
			printf (" -b, --ttl_base=n\t\t"
			"Time to live base: %d\n", DEFAULT_TTL_BASE);
			printf (" -x, --ttl_max=n\t\t"
			"Time to live max: %d\n", DEFAULT_TTL_MAX);			
			printf (" -c, --n_cpu=n\t\t\t"
			"Número de cpus: %d\n", DEFAULT_CPU);
			printf (" -k, --n_core=n\t\t\t"
			"Número de cores: %d\n", DEFAULT_CORE);
			printf (" -t, --n_thread=n\t\t"
			"Número de threads: %d\n", DEFAULT_THREAD);
			printf (" -p, --poolsize=n\t\t"
			"Tamaño de pool: %d\n", DEFAULT_POOLSIZE);
			printf (" -q, --queuesize=n\t\t"
			"Tamaño de cola: %d\n", DEFAULT_QUEUESIZE);
			printf (" -h, --help\t\t\t"
			"Ayuda\n");
			exit (2);
		default:
			fprintf(stderr, "Argumento desconocido, error\n");
			exit(4);
		} // switch
	} // while
	if(paramStruct.ttl_base > paramStruct.ttl_max)
	{
		printf("ttl base > ttl max ... invirtiendo valores\n");
		buff = paramStruct.ttl_base;
		paramStruct.ttl_base = paramStruct.ttl_max;
		paramStruct.ttl_max = buff;
	}
} // procesarParam
