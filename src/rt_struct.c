#include <unistd.h>
#include <stdio.h>
#include "estructuras.h"
#include "rt_struct.h"
#include "queue.h"

extern param_init_t paramStruct;


// bitmap


// inicializar struct
void init_rt_struct(rt_struct_t * _rt, int _n_of_queues){
    int i;
    unsigned long long int bitmap = 0;
    printf("%d longitud \n", sizeof(bitmap));
    _rt->size = _n_of_queues;
    _rt->queues = malloc( _n_of_queues * sizeof (queue_pcb_t) );
    for ( i = 0; i < _n_of_queues; i++ )
        inicializar_queue_pcb(&_rt->queues[i], paramStruct.nElementosCola);

}

// Mete un elemento PCB en la cola de la misma prioridad que el NICE del PCB
char rt_addPCB(rt_struct_t * _rt, pcb_t *_pcb){
    
    char ret = enqueue(&_rt->queues[_pcb->nice], _pcb);
    if(ret == 'y')
        _rt->bitmap_queue |= 0x0001 << _pcb->nice; 
    return ret;
}

// Extrae un elemento PCB de la cola con el indice _qIndx
pcb_t * takeFromQueue(rt_struct_t * _rt, int _qIndx){
    pcb_t ret*;

    if ( _qIndx >= _rt->size && _qIndx < 0) // Comprueba que esté dentro de los limites
    {
        fprintf(2, "Out of bounds %d\n", _qIndx);
        return NULL;
    }

    ret = dequeue(&_rt->queues[_qIndx]);
    if ( _rt->queues[_qIndx]->nElem == 0)
        _rt->bitmap_queue &=  ~(0 | (0x0001 << _qIndx)); // Poniendo bitmap como vacio

    if ( ret == NULL )
        fprintf(2, "De alguna forma ha llegado hasta aquí pidiendo un elemento pero no hay elementos en la cola\n");

    return ret;
}