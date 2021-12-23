#ifndef _QUEUE_H
#define _QUEUE_H

#include "estructuras.h"

typedef struct queue_pcb queue_pcb_t;
typedef struct queue_hueco queue_hueco_t;
typedef struct huecos_node huecos_node_t;


// Cola utilizada por el scheduler
typedef struct queue_pcb
{ 
	int nElem;
	int maxElem;
	int firstPos;
	int lastPos;
	pcb_t **malloc;
	pthread_mutex_t q_mtx;
} queue_pcb_t;

typedef struct huecos_node
{
    huecos_node_t *next, *prev;
    int size;
    int dir;
}huecos_node_t;

// Cola utilizada para huecos
typedef struct queue_hueco
{ 
	int nElem;
	int maxElem;
	int firstPos;
	int lastPos;
	huecos_node_t **malloc;
} queue_hueco_t;



void inicializar_queue_pcb(queue_pcb_t * _cola, int _maxElem);
char enqueue(queue_pcb_t *_q, pcb_t *_elem);
pcb_t * dequeue(queue_pcb_t *_cola);

void inicializar_queue_hueco(queue_hueco_t * _cola, int _maxElem);
char enqueueh(queue_hueco_t *_q, huecos_node_t *_elem);
huecos_node_t * dequeueh(queue_hueco_t *_cola);

#endif