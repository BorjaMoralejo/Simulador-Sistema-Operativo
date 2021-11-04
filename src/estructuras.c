#include "globals.h"
#include "estructuras.h"

//#include "defines.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


pthread_mutex_t mem_mtx;


// Inicializar maquinas
void init_machine(param_init_t *_params, matrix3_t *_matrix, machine_t *_machine){
	int i,j,k;
	cpu_t *cpu_l;
	core_t *core_l;
	thread_t *thread_l;
	matrix3_thread_t *thr_matrix;
	// Crear matriz tridimensional para los procesos
	(*_matrix) = malloc(sizeof(void *) * _params->n_cpu);
	thr_matrix = malloc(sizeof(void *) * _params->n_cpu);
	_machine->thread_matrix = thr_matrix;
	_machine->idle_threads = _params->n_cpu * _params->n_core * _params->n_thread;
	for(i = 0; i < _params->n_cpu; i++)
	{
		(*_matrix)[i] = malloc(sizeof(void *) * _params->n_core);
		thr_matrix[i] = malloc(sizeof(void *) * _params->n_core);

		for(j = 0; j < _params->n_core; j++)
		{
			(*_matrix)[i][j] = malloc(sizeof(void *) * _params->n_thread);
			thr_matrix[i][j] = malloc(sizeof(void *) * _params->n_thread);

		}
	}

	// Crear estructura de la máquina por si se va a utilizar en algún momento
	_machine->n_cpu = _params->n_cpu; 
	_machine->cpus = malloc(sizeof(cpu_t) * _params->n_cpu);
	_machine->pcb_matrix = _matrix;
	for (i = 0; i < _params->n_cpu; i++) // cpus
	{
		cpu_l = &_machine->cpus[i];
		cpu_l->id = i;
		cpu_l->machine = _machine;
		cpu_l->idle_threads = _params->n_core*_params->n_thread;
		cpu_l->cores = malloc(sizeof(core_t) * _params->n_core );
		for (j = 0; j < _params->n_core; j++)
		{
			core_l = &cpu_l->cores[j];
			core_l->id = i*_params->n_core + j;
			core_l->cpu = cpu_l;
			core_l->idle_threads = _params->n_thread;
			core_l->threads = malloc(sizeof(thread_t) * _params->n_thread);
			for(k = 0; k < _params->n_thread; k++)
			{
				thread_l = &core_l->threads[k];
				thread_l->id = i*_params->n_core + j*_params->n_thread + k;	
				thread_l->core = core_l;
				thread_l->enProceso = &((*_matrix)[i][j][k]);
				thr_matrix[i][j][k] = thread_l;
			}
		}
	}
	
}


// Inicializando pool principal de PCBs
void inicializar_estructura(int _maxElem){
	memPCBs.nElem = 0;
	memPCBs.maxElem = _maxElem;
	memPCBs.malloc = malloc(sizeof(pcb_t)*_maxElem);
	pthread_mutex_init(&mem_mtx, NULL);
}

// Inicializa la lista de la pool principal de pcbs
void inicializar_linkedList_int(lkdList_int_t *_cola, int _maxElem){
	printf("Inicializando cola de integers\n");
	int i;
	ll_node_int_t *arr_nodos;

	
	// crear _maxElem elementos de nodos de maxElem y asignar primero y último
	_cola->nElem = _maxElem;
	_cola->maxElem = _maxElem;
	arr_nodos = _cola->malloc = malloc(sizeof(ll_node_int_t)*_cola->maxElem);
	_cola->first = &arr_nodos[0];
	_cola->last = &arr_nodos[_cola->maxElem-1];
	
	// Linkear los nodos y asignar valores
	for( i = 0; i < _cola->maxElem-1; i++)
	{
		arr_nodos[i].sig = &arr_nodos[i+1];
		arr_nodos[i].q_int = i;
	}
	
	// Asignando el último elemento
	arr_nodos[_cola->maxElem - 1].q_int = _maxElem-1;
	arr_nodos[_cola->maxElem-1].sig = NULL;
}


// Saca un pcb de la pool principal
pcb_t * getPCB(){
	pthread_mutex_lock(&mem_mtx);
	pcb_t *_ret;
	lkdList_int_t *_cola_idx = &memPCBs.cola_idx;

	_cola_idx->nElem--;
	// Coge índice libre de mempool y avanza la lista
	ll_node_int_t * _indice = _cola_idx->first;
        _cola_idx->first = _indice->sig;
	// Coge el pcb que le toca
	_ret = &memPCBs.malloc[_indice->q_int];
	_ret->indice = _indice;
	_indice->sig = NULL;
	pthread_mutex_unlock(&mem_mtx);

	return _ret;
}

// devuelve el pcb a la pool principal y lo coloca en la ultima posicion
void putPCB(pcb_t * _elem){
	pthread_mutex_lock(&mem_mtx);
	ll_node_int_t * _ultimo = memPCBs.cola_idx.last;
	memPCBs.cola_idx.last = _elem->indice;
	_ultimo->sig = _elem->indice;
	memPCBs.nElem++;
	pthread_mutex_unlock(&mem_mtx);
}