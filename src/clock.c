#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "randomp.h"
#include "globals.h"
#include "queue.h"
#include "machine.h"

pthread_mutex_t timer_mtx;
pthread_cond_t timer_cond, br_timer_cond;
int nDone = 0, nTimers=2;
extern int used_mem;

void init_clock(){
    pthread_mutex_init(&timer_mtx, NULL);
    pthread_cond_init(&timer_cond, NULL);
    pthread_cond_init(&br_timer_cond, NULL);
}
void* start_clock(void * _args){
    int cpu, core, thread;
    int action;
    pcb_t * hilo;
    char state;
    while(1) // pulso
    { 
        
        // Sincronizar timers
        pthread_mutex_lock(&timer_mtx);
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n------------Pulso---------\n\n");
        printf("Memoria usada: %d (%d%%)\n", used_mem, used_mem * 100  / 12582912);
        printf("Idles: %d\t CPU \t CORE \t THREAD\t PID \t  Q\t  PRIO\n", machine.idle_threads);
        // avanzar tiempo de la maquina(cpu,cores,hilos)
        for(cpu = 0; cpu < paramStruct.n_cpu; cpu++)
            for(core = 0; core < paramStruct.n_core; core++)
                for(thread = 0; thread < paramStruct.n_thread; thread++)
                    if((hilo = matrix3[cpu][core][thread]) != NULL)
                    {
                        // Hacer operacion y comprobar si se
                        // 	1. continua normal
                        // 	2. termina la ejecucion
                        // 	3. se bloquea por "fallo en la cache" aleatorio
                        //
                        if(hilo->state == PCB_STATE_RUNNING)
                        {
                            action = do_command(&machine.cpus[cpu].cores[core].threads[thread]);
                            if(action == -1)
                            {
                                hilo->state = PCB_STATE_DEAD;
                                //printf("Terminando proceso %d\n", hilo->pid);
                            } else if (action == -2)
                            {
                                hilo->state = PCB_STATE_DEAD;
                                printf("Page Fault con el proceso %d!!\n", hilo->pid);
                            }

                            if (action == 1 && getRandom(0, paramStruct.block_chance) == 0) // !lucky
                            {
                                // SE BLOQUEA
                                hilo->blocked = paramStruct.max_blocked_time;
                                hilo->state = PCB_STATE_BLOCKED;
                            }
                        }
                        switch (hilo->state)    // BIRD
                        {
                        case PCB_STATE_BLOCKED: state='B';  break;
                        case PCB_STATE_IDLE:    state='I';  break;
                        case PCB_STATE_RUNNING: state='R';  break;
                        case PCB_STATE_DEAD:    state='D';  break;
                        default: state='U'; break;
                        }
                        printf("Proceso[%c] en \t%2d \t%2d \t%2d \t %d \t%3ld \t%3d\n",state,cpu,core,thread,
                                hilo->pid,hilo->q, hilo->priority );
                    }

        printf("\n\n-----Timers:\n");

        // Una vez los timers terminan, continuar
        while(nDone < nTimers){
            pthread_cond_wait(&timer_cond, &timer_mtx);
        }
        if(paramStruct.clock_manual == 1)
        {
            printf("Pulsa enter para continuar...\n");
            getchar();

        }else if(paramStruct.clock_retardado == 1)
        {
            usleep(paramStruct.freq_clock_ms*1000);   // Frecuencia del reloj
        }
        nDone = 0;
        // notificar a los timers
        pthread_cond_broadcast(&br_timer_cond);
        pthread_mutex_unlock(&timer_mtx);
    }
    
}
