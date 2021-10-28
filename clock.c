#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include "globals.h"
pthread_mutex_t timer_mtx;
pthread_cond_t timer_cond, br_timer_cond;
int nDone = 0, nTimers=2;

void init_clock(){
    pthread_mutex_init(&timer_mtx, NULL);
    pthread_cond_init(&timer_cond, NULL);
    pthread_cond_init(&br_timer_cond, NULL);
}
void* start_clock(void * _args){
    int cpu, core, thread;
    
    while(1) // pulso
    { 
        
        printf("------------Pulso---------\n\n\n\n\n");
        // avanzar tiempo de la maquina(cpu,cores,hilos)
        for(cpu = 0; cpu < paramStruct.n_cpu; cpu++)
            for(core = 0; core < paramStruct.n_core; core++)
                for(thread = 0; thread < paramStruct.n_thread; thread++)
                     if(matrix3[cpu][core][thread])
                     {
                         matrix3[cpu][core][thread]->ttl--;
                         printf("Reduciendo en %2d %2d %2d a PID=%d ttl=%4ld\n",cpu,core,thread,
                                    matrix3[cpu][core][thread]->pid, 
                                    matrix3[cpu][core][thread]->ttl);
                     }
        printf("\n\n-----Timers:\n");
        // Sincronizar timers
        pthread_mutex_lock(&timer_mtx);

        // Una vez los timers terminan, continuar
        while(nDone < nTimers){
            pthread_cond_wait(&timer_cond, &timer_mtx);
        }
        sleep(2);   // Frecuencia del reloj
        nDone = 0;
        // notificar a los timers
        pthread_cond_broadcast(&br_timer_cond);
        pthread_mutex_unlock(&timer_mtx);
    }
    
}