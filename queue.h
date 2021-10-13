#ifndef _QUEUE_H
#define _QUEUE_H

#include "estructuras.h"

void inicializar_queue_pcb(queue_pcb_t * _cola, int _maxElem);
char enqueue(queue_pcb_t *_q, pcb_t *_elem);
pcb_t * dequeue(queue_pcb_t *_cola);
#endif