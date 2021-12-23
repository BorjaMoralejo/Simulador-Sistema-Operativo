/* queue.c 
 * Asignatura: SESO
 * Autor: Borja Moralejo Tobajas
 * 
 * Fichero .c donde están las utilidades de la estructura de datos: cola FIFO
 */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


#include "queue.h"
#include "estructuras.h"
#include "physical.h"

extern pthread_mutex_t mem_mutex;


void inicializar_queue_pcb(queue_pcb_t * _cola, int _maxElem){
	int i;
	//printf("Inicializando cola de pcbs\n");
	// Se usa para inicializar las colas de los dispatchers/schedulers
	_cola->nElem = 0;
	_cola->maxElem = _maxElem;
	_cola->firstPos = 0; 
	_cola->lastPos = 0;
	_cola->malloc = malloc(_maxElem*sizeof( pcb_t* ));
	for( i = 0; i < _maxElem; i++)
		_cola->malloc[i] = (pcb_t *) '\0';	
	pthread_mutex_init(&_cola->q_mtx, NULL);
}

char enqueue(queue_pcb_t *_q, pcb_t *_elem){
	
	pthread_mutex_lock(&_q->q_mtx);
	if(_q->nElem >= _q->maxElem){
		printf("Ojo, la cola está llena! %d \n", _q->nElem);
		pthread_mutex_unlock(&_q->q_mtx);
		return 1; // está llena la cola
	}
	_q->malloc[_q->lastPos] = _elem;
	//printf("colocado en %d %ld \n",_q->lastPos, (long int) &_q->malloc[_q->lastPos]);
	_q->lastPos = (_q->lastPos +1) % _q->maxElem;
	_q->nElem++;
	pthread_mutex_unlock(&_q->q_mtx);
	return 0;
}
pcb_t * dequeue(queue_pcb_t *_cola){
	pthread_mutex_lock(&_cola->q_mtx);
	if(_cola->nElem == 0){
		//printf("COLA VACIA\n");
		pthread_mutex_unlock(&_cola->q_mtx);
		
		return NULL;
	}

	pcb_t * _ret = _cola->malloc[_cola->firstPos];
	_cola->nElem--;
	//printf("Pillando de %d %ld \n",_cola->firstPos,(long int) &_cola->malloc[_cola->firstPos]);
	_cola->firstPos = (_cola->firstPos +1) % _cola->maxElem;

	pthread_mutex_unlock(&_cola->q_mtx);

	return _ret;
}





void inicializar_queue_hueco(queue_hueco_t * _cola, int _maxElem){
	int i;
	//printf("Inicializando cola de pcbs\n");
	// Se usa para inicializar las colas de los dispatchers/schedulers
	_cola->nElem = _maxElem;
	_cola->maxElem = _maxElem;
	_cola->firstPos = 0; 
	_cola->lastPos = 0;
	_cola->malloc = malloc(_maxElem*sizeof( huecos_node_t* ));
	for( i = 0; i < _maxElem; i++)
		_cola->malloc[i] = malloc(sizeof(huecos_node_t));	
}

char enqueueh(queue_hueco_t *_q, huecos_node_t *_elem){
	
	if(_q->nElem >= _q->maxElem){
		return 1; // está llena la cola
	}
	_q->malloc[_q->lastPos] = _elem;
	//printf("colocado en %d %ld \n",_q->lastPos, (long int) &_q->malloc[_q->lastPos]);
	_q->lastPos = (_q->lastPos +1) % _q->maxElem;
	_q->nElem++;
	return 0;
}
huecos_node_t * dequeueh(queue_hueco_t *_cola){
	if(_cola->nElem == 0){
		return NULL;
	}
	huecos_node_t * _ret = _cola->malloc[_cola->firstPos];
	_cola->nElem--;
	//printf("Pillando de %d %ld \n",_cola->firstPos,(long int) &_cola->malloc[_cola->firstPos]);
	_cola->firstPos = (_cola->firstPos +1) % _cola->maxElem;
	return _ret;
}
