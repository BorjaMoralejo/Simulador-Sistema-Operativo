/*
Este fichero contiene las estructuras de datos usadas por el simulador de sistema operativo
*/
#ifndef _ESTRUCTURAS_H
#define _ESTRUCTURAS_H

struct pbs;
struct machine;
struct cpu;
struct core;
struct thread;

typedef int pid_t2;
typedef struct pcb pcb_t;

typedef struct lkdList_int lkdList_int_t;
typedef struct lkdList_node_int ll_node_int_t;
typedef struct queue_pcb queue_pcb_t;
typedef struct mempool_pcb mempool_pcb_t;

typedef struct machine machine_t;
typedef struct cpu cpu_t;
typedef struct core core_t;
typedef struct thread thread_t;
typedef struct param_init param_init_t;
typedef pcb_t **** matrix3_t;
typedef thread_t *** matrix3_thread_t; 

void init_machine(param_init_t *_params, matrix3_t *_matrix, machine_t *_machine);
void inicializar_estructura(int _maxElem);
void inicializar_linkedList_int(lkdList_int_t *_cola, int _maxElem);
pcb_t * getPCB();
void putPCB(pcb_t * _elem);


typedef struct param_init{
	int nElementosCola;
	int nPool;

	int clock_manual; 		// TODO: el clock avanza cada vez que el usuario de al enter
	int clock_retardado; 	// TODO: el clock avanza con el freq_clock_ms
	int freq_clock_ms; 		// TODO: frecuencia del reloj retardado

	int freq_disps_sched; 	// TODO: frecuencia del dispatcher
	
	int freq_pgen; 			// TODO: frecuencia del pgenerator
	int ttl_base;	 		// TODO: valor base o minimo del time to live
	int ttl_max;			// TODO: valor maximo del time to live

	int n_cpu;
	int	n_core;
	int n_thread;
}param_init_t;


typedef struct pcb
{
	pid_t2 pid;
	long int ttl;
	ll_node_int_t *indice; // Indice del gestor de memoria
} pcb_t;

typedef struct lkdList_int
{
	int nElem;
	int maxElem;
	ll_node_int_t *malloc;
	ll_node_int_t *first;
	ll_node_int_t *last;
} lkdList_int_t;

typedef struct lkdList_node_int
{
	ll_node_int_t *sig;
	int q_int;
} ll_node_int_t;

typedef struct queue_pcb
{ // Array circular, no nodos
	int nElem;
	int maxElem;
	int firstPos;
	int lastPos;
	pcb_t **malloc;
} queue_pcb_t;

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
} thread_t;

#endif