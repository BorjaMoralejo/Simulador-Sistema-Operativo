#include <stdio.h>

#include "estructuras.h"
#include "rt_struct.h"
#include "queue.h"
#include "list.h"
#include "scheduler.h"
#include "globals.h"


// Forward declaring
void return_pcb(sched_disp_data_t * _sched_d_d, pcb_t *_expired_pcb);
pcb_t * scheduler_get_next();

void dispatcher(sched_disp_data_t * sched_d_d){
    printf("Dispatcher haciendo cosas %d \n\n", sched_d_d->id);
    int thread;
    pcb_t **p_hilo, *next;
    core_t * assigned_core = sched_d_d->assigned_core;
    
    int context_change = 0;
    // Comprobar los procesos que estan en los hilos de la maquina
    // los procesos que hayan acabado sacarlos
    for (thread = 0; thread < paramStruct.n_thread; thread++)
    {
        // Pillando y reseteando variables cada bucle
        context_change = 0;

        p_hilo = assigned_core->threads[thread].enProceso;
        //printf("Trabajando con Hilo %d \n", thread);
        // Hay un proceso en este hilo, comprobar si ha terminado o si se ha quedado sin quantum
        if ( (*p_hilo) != NULL ) 
        {
            //printf("Hilo no es null \n");
            // Según la razón de entrada determinar si le toca cambiar de contexto o no
            if ( (*p_hilo)->ttl <= 0 )                          // Ha terminado
            {
                context_change = 1;
                printf("Sacando un proceso por finalización!\n");
                // Lo manda de vuelta a la memoria principal, el proceso ha terminado
                (*p_hilo)->state = PCB_STATE_DEAD;
                putPCB(*p_hilo);
            }
            else if ( (*p_hilo)->q <= 0)                        // Se ha quedado sin quantum
            {
                context_change = 1;
                printf("Sacando un proceso por quantum!\n");
                (*p_hilo)->state = PCB_STATE_IDLE;
                return_pcb(sched_d_d, *p_hilo);

            }else if ( (*p_hilo)->state == PCB_STATE_BLOCKED )  // Se ha autobloqueado
            {
                context_change = 1;
                printf("Sacando un proceso por autobloquearse!\n");
                if(list_addPCB(&sched_d_d->blocked_list, (*p_hilo)) != 0)
                {
                    fprintf(stderr, "La cola de pcb bloqueados está llena, a memoria principal \n");
                    putPCB((*p_hilo)); // Lo manda de vuelta a la memoria principal
                }
            }
            
            // Reduce el quantum
            (*p_hilo)->q--;

            // Deshace las redundancias al hacer cambio de contexto
            if(context_change == 1)
            {

                next = scheduler_get_next(sched_d_d);

                if (next == NULL)           // No hay siguiente, se vacia el hilo
                {
                    // Narcar como liberado y notificar a la máquina que tiene un hilo libre
                    *p_hilo = NULL;
                    assigned_core->idle_threads++;
                    assigned_core->cpu->idle_threads++;
                    machine.idle_threads++;
                    context_change = 0;
                }
                else if ( (*p_hilo) == next ) // El siguiente es el mismo proceso, no se realiza cambio de contexto
                {
                    context_change = 0;
                }

            }
        }else if ((*p_hilo) == NULL)    // El hilo estaba vacio, mete un PCB
        {
            printf("Hilo es null, cogiendo un PCB \n");
            next = scheduler_get_next(sched_d_d);
            if( next != NULL)
                context_change = 1;
        }


        if(context_change == 1)
        {
            printf("Cambiando de contexto \n");
            // Cambio de contexto del proceso anterior
            if((*p_hilo) != NULL && (*p_hilo)->state != PCB_STATE_DEAD)
            {
                // Guardando estado de la cpu
            }else 
            {
                machine.idle_threads--;
                assigned_core->idle_threads--;
                assigned_core->cpu->idle_threads--;
            }
            *p_hilo = next;           
            // Poner contexto de este proceso nuevo            
            printf("Asignando proceso en %2d %2d %2d \n", sched_d_d->cpu_id, sched_d_d->core_id, thread);


        }
    }
    printf("Dispatcher ha terminado de hacer cosas %d\n\n", sched_d_d->id);

}



void return_pcb(sched_disp_data_t * _sched_d_d, pcb_t *_expired_pcb){
    rt_struct_t * rt = _sched_d_d->rt;
    rt_struct_t * rt_expired = _sched_d_d->rt_expired;
   // tree_t * arbol;
    
    // Arbol, no implementado pero casi
    /*
    if(_expired_pcb->nice >= rt_expired->size)    // Al arbol
    {
        if ( addNode(arbol, _expired_pcb) != 0) 
        {
            fprintf(stderr, "Al parecer no hay nodos, así que a Brasil que te vas \n");
            putPCB(_expired_pcb); // Lo manda de vuelta a la memoria principal
        }
    }
    */
    if(_expired_pcb->nice < rt_expired->size)  // Al RT_struct, aplicando degradación paulatina
    {        
        if(_expired_pcb->nice != rt_expired->size)
            _expired_pcb->nice++;

        if ( rt_addPCB( rt_expired, _expired_pcb) != 'y') 
        {
            fprintf(stderr, "Al parecer esa cola estaba llena o algo, así que a Brasil que te vas \n");
            putPCB(_expired_pcb); // Lo manda de vuelta a la memoria principal
        }
    }
    else 
    {
        fprintf(stderr, "Fuera de rango de prioridades, no esta implementado\n");
        putPCB(_expired_pcb); // Lo manda de vuelta a la memoria principal
    }
}
