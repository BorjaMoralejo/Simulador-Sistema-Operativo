/* queue.c 
 * Asignatura: SO
 * Autor: Borja Moralejo Tobajas
 * 
 * Fichero .c donde están las utilidades de la estructura de datos: cola FIFO
 */
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

void inicializar_queue_pcb(queue_pcb_t * _cola, int _maxElem){
	int i;
	#ifdef PRINTF_EN
	printf("Inicializando cola de pcbs\n");
	#endif
	// Se usa para inicializar las colas de los dispatchers/schedulers
	_cola->nElem = 0;
	_cola->maxElem = _maxElem;
	_cola->firstPos = 0; 
	_cola->lastPos = 0;
	_cola->malloc = malloc(_maxElem*sizeof( pcb_t* ));
	for( i = 0; i < _maxElem; i++)
		_cola->malloc[i] = (pcb_t *) '\0';	
	
}

char enqueue(queue_pcb_t *_q, pcb_t *_elem){
	if(_q->nElem >= _q->maxElem){
		printf("Ojo, la cola está llena! %d \n", _q->nElem);
		return 0; // está llena la cola
	}
	_q->malloc[_q->lastPos] = _elem;
	printf("colocado en %d %ld \n",_q->lastPos, (long int) &_q->malloc[_q->lastPos]);
	_q->lastPos = (_q->lastPos +1) % _q->maxElem;
	_q->nElem++;
}
pcb_t * dequeue(queue_pcb_t *_cola){
	if(_cola->nElem == 0){
		printf("COLA VACIA\n");
		
		return '\0';
	}
	pcb_t * _ret = _cola->malloc[_cola->firstPos];
	_cola->nElem--;
	printf("Pillando de %d %ld \n",_cola->firstPos,(long int) &_cola->malloc[_cola->firstPos]);
	_cola->firstPos = (_cola->firstPos +1) % _cola->maxElem;
	//printf("%d\n",_cola->firstPos);
	return _ret;
}
