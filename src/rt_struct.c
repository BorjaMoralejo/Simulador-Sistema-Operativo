#include <unistd.h>
#include <stdio.h>
#include "estructuras.h"
#include "queue.h"

extern param_init_t paramStruct;


// bitmap
char *bitmap_queue;
queue_pcb_t *queues;
int N_OF_RTSIZE = 30;


// inicializar struct
void init_rt_struct(){
    int i;
    unsigned long long int bitmap = 0;
    printf("%d longitud \n", sizeof(bitmap));

    queues = malloc( N_OF_RTSIZE * sizeof (queue_pcb_t) );
    for ( i = 0; i < N_OF_RTSIZE; i++ )
        inicializar_queue_pcb(&queues[i], paramStruct.nElementosCola);

}

// Mete un elemento PCB en la cola de la misma prioridad que el NICE del PCB
char addPCB(pcb_t *_pcb){
    return enqueue(&queues[_pcb->nice], _pcb);
}

// Extrae un elemento PCB de la cola con el indice _qIndx
pcb_t *pcb takeFromQueue(int _qIndx){
    pcb_t ret*;

    if ( _qIndx >= N_OF_RTSIZE && _qIndx < 0) // Comprueba que esté dentro de los limites
    {
        fprintf(2, "Out of bounds %d\n", _qIndx);
        return NULL;
    }

    ret = dequeue(&queues[_qIndx]);

    if ( ret == NULL )
        fprintf(2, "De alguna forma ha llegado hasta aquí pidiendo un elemento pero no hay elementos en la cola\n");

    return ret;
}