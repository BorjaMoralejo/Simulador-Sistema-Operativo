#include <pthread.h>
#include <stdio.h>
#include "defines.h"
#include <unistd.h>

extern pthread_cond_t timer_cond, br_timer_cond;
extern pthread_mutex_t timer_mtx;
extern int nDone;

unsigned int tick_freq_dispsched = 10;
unsigned int tick_freq_pgenerator = 10;
typedef void(*func_t)(int);

void timer_DispSched(int _time);
void timer_PGenerator(int _time);

void start_timer(short int _id, short int _function){
    func_t func_timer;
    unsigned int localTime = 0 ;
    switch (_function)
    {
        case DISPATCHER_SCHEDULER_FUNC:
            func_timer = &timer_DispSched;
        break;
        case PGENERATOR_FUNC:
            func_timer = &timer_PGenerator;
        break;
        default:
            
        break;
    }
    printf("Timer en marcha\n");


    // Control de sincronización
    pthread_mutex_lock(&timer_mtx);
    printf("Pasando lock\n");

    while(1)
    {
        // ver si tiene que hacer tick
        localTime++; // adios localidad de la cache
        // do stuff con una funcion virtual
        // por ejemplo, decirle al scheduler cada X tiempo que despierte
        // decirle a pgenerator que haga algo
        printf("tick?%d\n", localTime);
        sleep(1);
        (*func_timer)(localTime);

        nDone++;
        pthread_cond_signal(&timer_cond);
        pthread_cond_wait(&br_timer_cond, &timer_mtx);
    }  
}


extern int sched_flag;
extern pthread_mutex_t sched_mtx;
extern pthread_cond_t sched_cond, br_sched_cond;

void timer_DispSched(int _time){
    if(_time % tick_freq_dispsched == 0) // tick scheduler
    {
        // via libre al scheduler
        printf("tick dispatcher\n");
      //  while(sched_flag != 1)
       //     pthread_cond_wait(&sched_cond, &sched_mtx);
       // sched_flag = 0;
        // esperar a que el scheduler haya terminado
       // pthread_cond_broadcast(&br_sched_cond);
       // pthread_mutex_unlock(&sched_mtx);
        // esperar a que el scheduler haya terminado
        // bloquearlo otra vez

    }
}

void timer_PGenerator(int _time){
    if(_time % tick_freq_pgenerator == 0) // tick pgenerator
    {
        printf("tick generator\n");
        // Via libre a generar proceso
     //   while(sched_flag != 1)
       //     pthread_cond_wait(&sched_cond, &sched_mtx);
        //sched_flag = 0;
        // esperar a que el scheduler haya terminado
       // pthread_cond_broadcast(&br_sched_cond);
       // pthread_mutex_unlock(&sched_mtx);
        // bloquearlo
    }
}

