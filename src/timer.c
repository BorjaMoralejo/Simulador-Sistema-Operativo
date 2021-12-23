#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "globals.h"
#include "defines.h"

extern pthread_cond_t timer_cond, br_timer_cond;
extern pthread_mutex_t timer_mtx;
extern int nDone;

typedef void(*func_t)(int);

void timer_DispSched(int _time);
void timer_PGenerator(int _time);
void timer_loader(int _time);

void* start_timer(void * _args){
    short int * punt = (short *) _args;
    short int id = punt[0], function = punt[1];
    func_t func_timer;
    unsigned int localTime = 0;
    pthread_mutex_t local_mutex;
    pthread_cond_t local_cond;


    // Asignar función virtual
    switch (function)
    {
        case DISPATCHER_SCHEDULER_FUNC:
            func_timer = &timer_DispSched;
        break;
        //case PGENERATOR_FUNC:
           // func_timer = &timer_PGenerator;
        case LOADER_FUNC:
            func_timer = &timer_loader;
        break;
        default:
            while (1)
            {
                printf("Aqui algo raro está pasando\n");
            }
            
        break;
    }
    printf("Timer en marcha\n");


    // Control de sincronización
    pthread_mutex_lock(&timer_mtx);
    printf("Pasando lock\n");
    pthread_mutex_unlock(&timer_mtx);

    while(1)
    {
        // ver si tiene que hacer tick
        localTime++;

        // do stuff con una funcion virtual asignada
        (*func_timer)(localTime);

        nDone++;
        pthread_cond_signal(&timer_cond);
        pthread_cond_wait(&br_timer_cond, &timer_mtx);
    }  
}


extern int sched_flag;
extern pthread_mutex_t sched_mtx;
extern pthread_cond_t sched_cond;

void timer_DispSched(int _time){

    if(_time % paramStruct.freq_disps_sched == 0) // tick scheduler
    {
        printf("tick dispatcher%d?\n", _time);

        // Se comunica con scheduler y le da luz verde
        sched_flag = 0;
        pthread_cond_signal(&sched_cond);

        // Esperar a que el scheduler haya terminado
        while(sched_flag == 0);     
        printf("tick dispatcher%d!\n", _time);
    }
}
/*
extern int pgen_flag;
extern pthread_mutex_t pgen_mtx;
extern pthread_cond_t pgen_cond;

void timer_PGenerator(int _time){
    
    if(_time % paramStruct.freq_pgen == 0) // tick pgenerator
    {
        printf("tick generator%d?\n", _time);
        // Via libre a generar proceso

        // Se comunica con pgenerator y le da luz verde
        pgen_flag = 0;
        pthread_cond_signal(&pgen_cond);

        // Esperar a que el pgenerator haya terminado
        while(pgen_flag == 0);  

        printf("tick generator%d!\n", _time);

    }
}
*/
extern int loader_flag;
extern pthread_mutex_t loader_mtx;
extern pthread_cond_t loader_cond;

void timer_loader(int _time){
    
    if(_time % paramStruct.freq_pgen == 0) // tick pgenerator
    {
        printf("tick generator%d?\n", _time);
        // Via libre a generar proceso

        // Se comunica con pgenerator y le da luz verde
        loader_flag = 0;
        pthread_cond_signal(&loader_cond);

        // Esperar a que el pgenerator haya terminado
        while(loader_flag == 0);  

        printf("tick generator%d!\n", _time);

    }
}