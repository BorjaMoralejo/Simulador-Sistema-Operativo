/*
Este fichero contiene las estructuras de datos usadas por el simulador de sistema operativo
*/


struct pbs;
struct machine;
struct cpu;
struct core;
struct thread;

typedef int pid_t;
typedef struct pcb pcb_t;
typedef struct queue_int queue_int_t;
typedef struct queue_node_int q_node_int_t;
typedef struct queue_pcb queue_pcb_t;
typedef struct mempool_pcb mempool_pcb_t;
typedef struct machine machine_t;
typedef struct cpu cpu_t;
typedef struct core core_t;
typedef struct thread thread_t;

typedef struct pcb{
	pid_t pid;
	unsigned long int ttl;
	q_node_int_t *indice; // Indice del gestor de memoria
}pcb_t;


typedef struct queue_int{
	int nElem;
	int maxElem;
	q_node_int_t * malloc;
	q_node_int_t * first;
	q_node_int_t * last;
}queue_int_t;

typedef struct queue_node_int{
	q_node_int_t *sig;
	int q_int;
}q_node_int_t;


typedef struct queue_pcb{ // Array circular, no nodos
	int nElem;
	int maxElem;
	int firstPos;
	int lastPos;
	pcb_t **malloc;
}queue_pcb_t;

typedef struct mempool_pcb{
	int nElem;
	int maxElem;
	queue_int_t cola_idx; // Cola índices
	pcb_t* malloc; // pcbs en memoria
}mempool_pcb_t;



typedef struct machine{
	int n_cpu;
	cpu_t *cpus;
}machine_t;

typedef struct cpu{
	int id;
	machine_t *machine;
}cpu_t;

typedef struct core{
	int id;
	cpu_t *cpu;
}core_t;

typedef struct thread{
	int id;
	core_t *core;
	pcb_t *enProceso;
}thread_t;
