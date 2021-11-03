#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "estructuras.h"
#include "queue.h"
int sched_flag = 0;
pthread_mutex_t sched_mtx;
pthread_cond_t sched_cond;

void init_scheduler()
{
    pthread_mutex_init(&sched_mtx, NULL);
    pthread_cond_init(&sched_cond, NULL);
}

void *start_schedule(void *_args)
{
    int cpu, core, thread;
    int flag_found, contador, contador_max;
    contador_max = paramStruct.n_cpu * paramStruct.n_core * paramStruct.n_thread;
    pcb_t *dequeing;
    pcb_t **p_hilo;
    thread_t *hilo;
    while (1)
    {
        pthread_mutex_lock(&sched_mtx);
        sched_flag = 1;
        // esperando al timer
        while (sched_flag == 1)
            pthread_cond_wait(&sched_cond, &sched_mtx);

        // printear que tiene que hacer algo
        printf("Don't mind me, Dispatcher/Scheduler haciendo cosas\n");
        // mira los procesos que tiene fuera de la cola y los mete en la cola o algo
        usleep(10); // funcion para simular que esta haciendo algo
        
        // Comprobar los procesos que estan en los hilos de la maquina
        // los procesos que hayan acabado sacarlos
        for (cpu = 0; cpu < paramStruct.n_cpu; cpu++)
            for (core = 0; core < paramStruct.n_core; core++)
                for (thread = 0; thread < paramStruct.n_thread; thread++)
                {
                    p_hilo = &matrix3[cpu][core][thread];
                    if (matrix3[cpu][core][thread] && (*p_hilo)->ttl <= 0)
                    {
                        printf("Sacando un proceso por finalización!\n");
                        putPCB(*p_hilo);
                        // marcar como liberado y notificar a la máquina que tiene un hilo libre
                        *p_hilo = NULL;
                        //hilo = *(machine.thread_matrix)[cpu][core][thread];
                        //hilo->core->idle_threads++;
                        //hilo->core->cpu->idle_threads++;
                        machine.cpus[cpu].idle_threads++;
                        machine.cpus[cpu].cores[core].idle_threads++;
                        machine.idle_threads++;
                    }
                }
        // Esto para cuando cpu o lo que sea tenga su scheduler
            // si no tiene proceso y hay en la cola, meterlo 
            // cada scheduler tiene su cola de prioridades?

        // Sacar PCB de la cola y meterlo en la máquina
        while (machine.idle_threads > 0 && (dequeing = dequeue(&q_pcb)))
        {   
            flag_found = 0;
            contador = 0;
            while(flag_found == 0 && contador <= contador_max)
            {
                cpu = contador/(paramStruct.n_core*paramStruct.n_thread);
                core = (contador%(paramStruct.n_core*paramStruct.n_thread))/(paramStruct.n_thread);
                thread = contador%paramStruct.n_thread;
                if(!matrix3[cpu][core][thread])
                {
                    matrix3[cpu][core][thread] = dequeing;
                    machine.idle_threads--;
                    machine.cpus[cpu].idle_threads--;
                    machine.cpus[cpu].cores[core].idle_threads--;
                    printf("Asignando proceso en %2d %2d %2d \n", cpu, core, thread);
                    flag_found = 1;
                }else contador++;
            }// while busqueda

        } // while cola

        pthread_mutex_unlock(&sched_mtx);
    }
}