#include "estructuras.h"
//#include "defines.h"
#include <stdio.h>
#include <unistd.h>
//mempool_t memPCBs;

void inicializar_estructura(mempool_pcb_t *_mempool, int _maxElem){
	int N_PBS_MAX;
	_mempool->nElem = 0;
	_mempool->maxElem = _maxElem;
	_mempool->malloc = malloc(sizeof(pcb_t)*_maxElem);
	//_mempool->cola_idx = malloc(sizeof(queue_int_t));
	_mempool->cola_idx.nElem = _maxElem;
	_mempool->cola_idx.maxElem = _maxElem;
	//pcb_t *memoriaPrincipal = malloc(sizeof(pcb_t)*N_PBS_MAX);

}

void inicializar_queue_int(queue_int_t *_cola){
//	printf("Inicializando cola de integers\n");
	if(_cola == NULL)
	{
		printf("La cola es nula!\n");
		_exit(1);
	}
	
	
//	printf("%d", _cola->maxElem);
	// crear n elementos de nodos de maxElem y asignar primero y último
	 //q_node_int_t * arrQNT = malloc(sizeof(q_node_int_t)*_cola->maxElem);
	_cola->malloc = malloc(sizeof(q_node_int_t)*_cola->maxElem);
	//printf("no");
	q_node_int_t * arrQNT = _cola-> malloc;
	_cola->first = &arrQNT[0];
	_cola->last = &arrQNT[_cola->maxElem-1];
	// Linkear los nodos y asignar valores
	for(int i = 0; i < _cola->maxElem-2; i++)
	{
		arrQNT[i].sig = &arrQNT[i+1];
		arrQNT[i].q_int = i;
	}
	// Asignando el último elemento
	arrQNT[_cola->maxElem-1].q_int = _cola->maxElem;
	arrQNT[_cola->maxElem-1].sig = NULL;


//	printf("Inicialización de cola de integers finalizada\n");
}


void inicializar_queue_pcb(queue_pcb_t * _cola, int _maxElem){
//	printf("Inicializando cola de pcbs\n");
	// Se usa para inicializar las colas de los dispatchers/schedulers
//	if(_cola == NULL)
//		printf("NO\n");
	//_cola = malloc(sizeof(queue_pcb_t));
	_cola->nElem = 0;
	_cola->maxElem = _maxElem;
	_cola->firstPos = 0; 
	_cola->lastPos = 0;
//	printf("Hola %ld\n", sizeof(pcb_t *)*_maxElem);
	_cola->malloc = malloc(_maxElem*sizeof( pcb_t* ));
//	printf("fug \n");
	for(int i = 0; i < _maxElem; i++){
			
		//printf("%ld\n", &_cola->malloc[i]);
		_cola->malloc[i] = (pcb_t *) NULL;	
		//printf("%ld\n", &_q.malloc[i]); 
	}
	//_q.first = _q.malloc[_maxElem]; 
}

char enqueue(queue_pcb_t *_q, pcb_t *_elem){
	if(_q->nElem >= _q->maxElem){
		printf("Ojo, la cola está llena! %d \n", _q->nElem);
		return 0; // está llena la cola
	}
	_q->malloc[_q->lastPos] = _elem;
	printf("colocado en %d %ld \n",_q->lastPos, &_q->malloc[_q->lastPos]);
	_q->lastPos = (_q->lastPos +1) % _q->maxElem;
	_q->nElem++;
}
pcb_t * dequeue(queue_pcb_t *_cola){
	if(_cola->nElem == 0){
		printf("NULL\n");
		return NULL;
	}
	pcb_t * _ret = _cola->malloc[_cola->firstPos];
	_cola->nElem--;
	printf("Pillando de %d %ld \n",_cola->firstPos, &_cola->malloc[_cola->firstPos]);
	_cola->firstPos = (_cola->firstPos +1) % _cola->maxElem;
	printf("%d\n",_cola->firstPos);
	return _ret;
}

pcb_t * getPCB(mempool_pcb_t * memPCBs){
	pcb_t *_ret;
	queue_int_t * _cola_idx = &memPCBs->cola_idx;
	_cola_idx->nElem--;
	// Coge índice libre de mempool y avanza la lista
	q_node_int_t * _indice = _cola_idx->first;
        _cola_idx->first = _indice->sig;
	// Coge el pcb que le toca
	_ret = &memPCBs->malloc[_indice->q_int];
	_ret->indice = _indice;
	return _ret;
}

void putPCB(mempool_pcb_t * memPCBs, pcb_t * _elem){
	q_node_int_t *_node = memPCBs->cola_idx.last;
}