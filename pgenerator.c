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
    pid_t2 pid_count = 1;
    pcb_t * pcb;
    

    while (1)
    {
        pthread_mutex_lock(&pgen_mtx);
        pgen_flag = 1;
        // esperando al timer
        while(pgen_flag == 1)
            pthread_cond_wait(&pgen_cond, &pgen_mtx);

        printf("Generando procesos\n");
        // genera proceso aleatorio
        pcb = getPCB();
        if(pcb != NULL)
        {
            pcb->pid = pid_count++;
            pcb->ttl = (long) getRandom(1, 30);
            if (enqueue(&q_pcb, pcb) == 'n')
            {
                // La cola estaba llena
            }else {
                // Todo bien, hacer lo que sea
            }
        }
		

        // lo añade a la cola?
        // o se lo pasa a scheduler?

        usleep(10); // funcion para simular que esta haciendo algo
        // cada scheduler tiene su cola de prioridades?        

        pthread_mutex_unlock(&pgen_mtx);
    }
}