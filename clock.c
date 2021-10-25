#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

pthread_mutex_t timer_mtx;
pthread_cond_t timer_cond, br_timer_cond;
int nDone = 0, nTimers=1;

void init_clock(){
    pthread_mutex_init(&timer_mtx, NULL);
    pthread_cond_init(&timer_cond, NULL);
    pthread_cond_init(&br_timer_cond, NULL);
}
void* start_clock(void * _args){

    while(1) // pulso
    { 
        // avanzar tiempo de la maquina(cpu,cores,hilos)

        // Sincronizar timers
        pthread_mutex_lock(&timer_mtx);
        printf("Pulso\n");

        // Una vez los timers terminan, continuar
        while(nDone < nTimers){
            pthread_cond_wait(&timer_cond, &timer_mtx);
        }
        nDone = 0;
        // notificar a los timers
        pthread_cond_broadcast(&br_timer_cond);
        pthread_mutex_unlock(&timer_mtx);
        sleep(1);   
    }
    
}