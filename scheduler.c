#include <pthread.h>
#include <stdio.h>

int sched_flag = 0;
pthread_mutex_t sched_mtx;
pthread_cond_t sched_cond, br_sched_cond;

void init_scheduler(){
    pthread_mutex_init(&sched_mtx, NULL);
    pthread_cond_init(&sched_cond, NULL);
    pthread_cond_init(&br_sched_cond, NULL);
}

void scheduleFunc(){
    pthread_mutex_lock(&sched_mtx);
    while (1)
    {
        sched_flag = 1;
        // printear que tiene que hacer algo
        printf("Dispatcher/Scheduler haciendo cosas\n");
        // mira los procesos que tiene fuera de la cola y los mete en la cola o algo

        // Comprobar los procesos que estan en los hilos de la maquina
        // los procesos que hayan acabado sacarlos
        // si no tiene proceso y hay en la cola, meterlo

        // cada scheduler tiene su cola de prioridades?
        pthread_cond_signal(&sched_cond);
        pthread_cond_wait(&br_sched_cond, &sched_mtx);
    }

}