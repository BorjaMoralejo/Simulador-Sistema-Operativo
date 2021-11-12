#ifndef _RT_STRUCT_H
#define _RT_STRUCT_H 1

#include "estructuras.h"

// inicializar struct
void init_rt_struct();

// meter pcb y que mire su nice
void addPCB(pcb_t *_pcb);

// sacar pcb de cola
pcb_t *pcb takeFromQueue(int _queue);

#endif