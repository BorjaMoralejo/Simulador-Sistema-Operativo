/*
Este fichero contiene las estructuras de datos usadas por el simulador de sistema operativo
*/
#ifndef _ESTRUCTURAS_H
#define _ESTRUCTURAS_H
#include <pthread.h>
#include "tlb.h"
#define PCB_STATE_IDLE 		0
#define PCB_STATE_RUNNING 	1
#define PCB_STATE_BLOCKED 	2
#define PCB_STATE_DEAD 		3

typedef int pid_t2;
typedef struct pcb pcb_t;

typedef struct lkdList_int lkdList_int_t;
typedef struct lkdList_node_int ll_node_int_t;
typedef struct mempool_pcb mempool_pcb_t;

typedef struct entrada_tlb entrada_tlb_t;
typedef struct machine machine_t;
typedef struct cpu cpu_t;
typedef struct core core_t;
typedef struct thread thread_t;
typedef struct param_init param_init_t;
typedef struct mm_struct mm_t;
typedef struct mmu_struct mmu_t;

typedef struct status status_t;


typedef pcb_t **** matrix3_t;
typedef thread_t *** matrix3_thread_t; 

void init_machine(param_init_t *_params, matrix3_t *_matrix, machine_t *_machine);
void inicializar_estructura(int _maxElem);
void inicializar_linkedList_int(lkdList_int_t *_cola, int _maxElem);
pcb_t * getPCB();
void putPCB(pcb_t * _elem);

// Estructura que contiene las variables parametrizables del simulador
typedef struct param_init{
	int nElementosCola;
	int nPool;

	int clock_manual; 		// el clock avanza cada vez que el usuario de al enter
	int clock_retardado; 	// el clock avanza con el freq_clock_ms
	int freq_clock_ms; 		// frecuencia del reloj retardado

	int freq_disps_sched; 	// frecuencia del dispatcher
	
	int freq_pgen; 			// frecuencia del pgenerator
	int ttl_base;	 		// valor base o minimo del time to live
	int ttl_max;			// valor maximo del time to live

	int blocked_list_size;	// Tamaño maximo de la lista de bloqueado
	int block_chance;		// Probabilidad de que se bloquee, más alto, mas raro de que eso ocurra
	int max_blocked_time;	// Numero de ciclos maximos que se quedan bloqueados los procesos
	int freq_reschedule;	// Frecuencia a la que se recalcula las colas de prioridades
	int quantum_per_prio;	// Cantidad de quantum por cada nivel de prioridad. Escala linealmente
	int max_prio;			// Nivel de prioridad máximo para la tabla de prioridades.
	int random_priority;	// Nivel máximo de prioridad que se puede asignar de forma aleatoria. Desde 0 hasta valor introducido
	int random_affinity;	// Poner afinidad aleatoria o no
	int pcb_generated;		// Numero de procesos generados por tick de PGenerator

	int politica_scheduler_master; // RoundRobin, affinity

	int n_cpu;
	int	n_core;
	int n_thread;
}param_init_t;

// Estructura para las estadisticas

// Estructura principal del simulador, procesos
typedef struct pcb
{
	ll_node_int_t *indice; // Indice del gestor de memoria, no esta relacionado con la simulación
	pid_t2 pid;
	long int ttl;
	char state;
	int affinity;
	int priority;
	long int q;
	long int blocked;
	mm_t mm;
	status_t status;

	// estructura mm
	// estructura status

} pcb_t;

// Estructura utilizada para guardar el contexto de los procesadores
typedef struct status
{
	int *rn; 	// Array de 16 elementos
	int pc; 	// 
	int ri;		// Registro de instrucción
	//int PTBR;	// 
	

}status_t;


// Estructura utilizada para guardar las direcciones virtuales de las instrucciones y datos del proceso.
typedef struct mm_struct
{
	//unsigned int start_dir;	// Inicio dirección virtual
	unsigned int code_p; 	// Dirección virtual
	unsigned int data_p; 	// Dirección virtual
	unsigned int end_p;		// Dirección final de data
	unsigned int *pgb;		// Dirección física inicial del programa
}mm_t;



// Estructura de linked list de enteros
typedef struct lkdList_int
{
	int nElem;
	int maxElem;
	ll_node_int_t *malloc;
	ll_node_int_t *first;
	ll_node_int_t *last;
} lkdList_int_t;

// Nodos de la lista de enteros
typedef struct lkdList_node_int
{
	ll_node_int_t *sig;
	int q_int;
} ll_node_int_t;



typedef struct mempool_pcb
{
	int nElem;
	int maxElem;
	lkdList_int_t cola_idx; // Cola índices
	pcb_t *malloc;			// pcbs en memoria
} mempool_pcb_t;

typedef struct machine
{
	matrix3_t *pcb_matrix;
	matrix3_thread_t *thread_matrix;
	int n_cpu;
	cpu_t *cpus;
	int idle_threads;
} machine_t;

typedef struct cpu
{
	int id;
	machine_t *machine;
	core_t *cores;
	int idle_threads;
} cpu_t;

typedef struct core
{
	int id;
	cpu_t *cpu;
	thread_t *threads;
	int idle_threads;
} core_t;

typedef struct thread
{
	int id;
	core_t *core;
	pcb_t **enProceso;


	int *rn; 	// Array de 16 elementos
	int pc; 	// 
	int ri;		// Registro de instrucción
	int *PTBR;	// Puntero a pagetable del pcb 
	mmu_t *mmu;


} thread_t;

#endif