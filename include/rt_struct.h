#ifndef _RT_STRUCT_H
#define _RT_STRUCT_H 1

#include "estructuras.h"


typedef struct rt_struct rt_struct_t;
// inicializar struct
void init_rt_struct(rt_struct_t * _rt, int _n_of_queues);

// meter pcb y que mire su nice
char rt_addPCB(rt_struct_t * _rt, pcb_t *_pcb);

// sacar pcb de cola
pcb_t * takeFromQueue(rt_struct_t * _rt, int _qIndx);

typedef struct rt_struct {
    unsigned long bitmap_queue;
    queue_pcb_t *queues;
    int size;
} rt_struct_t;

#endif