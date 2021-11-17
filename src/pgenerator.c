#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "randomp.h"
#include "globals.h"
#include "estructuras.h"
#include "queue.h"


int pgen_flag = 0;
pthread_mutex_t pgen_mtx;
pthread_cond_t pgen_cond;

void init_pgen(){
    pthread_mutex_init(&pgen_mtx, NULL);
    pthread_cond_init(&pgen_cond, NULL);
}

void * start_pgenerator(void * _args){    
    pid_t2 pid_count = 0;
    pcb_t * pcb;
    int i;    

    while (1)
    {
        pthread_mutex_lock(&pgen_mtx);
        pgen_flag = 1;
        // esperando al timer
        while(pgen_flag == 1)
            pthread_cond_wait(&pgen_cond, &pgen_mtx);

        printf("Generando procesos\n");

        // Coge proceso de la memoria del simulador (no es parte de la simulación)
        for ( i = 0; i < paramStruct.pcb_generated; i++)
        {
            pcb = getPCB();
            if(pcb != NULL)
            {
                if (paramStruct.random_priority == -1)
                    pcb->priority = 0;
                else pcb->priority = getRandom(0, paramStruct.random_priority);

                if (paramStruct.random_affinity)
                    pcb->affinity = getRandom(0, paramStruct.n_core)+getRandom(0, paramStruct.n_cpu)*paramStruct.n_core;
                else 
                    pcb->affinity = -1;

                pcb->pid = pid_count++;
                pcb->ttl = (long) getRandom(paramStruct.ttl_base, paramStruct.ttl_max);

                // Se lo pasa al scheduler maestro
                if (enqueue(&q_pcb, pcb) == 1)
                {
                    // La cola estaba llena, devolver a la memoria principal
                    putPCB(pcb);
                }else {
                    // Todo bien, hacer lo que sea
                }
            }
        }

        pthread_mutex_unlock(&pgen_mtx);
    }
}