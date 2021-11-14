#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "scheduler.h"
#include "dispatcher.h"
#include "globals.h"
#include "estructuras.h"
#include "queue.h"
#include "list.h"
#include "rt_struct.h"


int sched_flag = 0;
pthread_mutex_t sched_mtx;
pthread_cond_t sched_cond;

// Variables para los multiples schedulers
pthread_cond_t multi_sched_cond, br_multi_sched_cond;
pthread_mutex_t multi_sched_mtx;
int sched_done;

static int sched_id; 

//int prio_to_weight *; // TODO: al inicializar el programa generar esto

// Inicializar las estructuras de cada uno de los schedulers.
// Cada scheduler llama a esta subrutina y se crea un "objeto" de scheduler de esta forma.
void init_scheduler(sched_disp_data_t * _sched_d_d)
{
    pthread_mutex_init(&sched_mtx, NULL);
    pthread_cond_init(&sched_cond, NULL);
    // TODO:
    _sched_d_d->id = sched_id++;
    // init rt y rt expired
    // init colas de bloqueado y procesos nuevos

}


// Cada X ciclos se recalculan los parametros del scheduler y está es la función que lo hace
void reschedule(sched_disp_data_t * _shed_d_d){
    pcb_t * buf;
    queue_pcb_t tmp;

    int prio = _shed_d_d->actual_prio;
    int size = _shed_d_d->rt->size;

    // los procesos que estaban en proceso se dejan porque van a ir a la cola expirado igualmente

    // Se pasa todo del rt a expired, eso solo con la actual, con las siguientes colas no vacias se swapean de sitio
    if(prio < size)
    while ((buf = dequeue (_shed_d_d->rt->queues[prio])) != NULL)
        rt_addPCB(_shed_d_d->rt_expired, buf);
    
    prio++;
    // Se traspasan las colas a la estructura expirada 
    while ( (_shed_d_d->rt->bitmap_queue & (0x1 << prio)) == 0  && prio < size)
    {
        tmp = _shed_d_d->rt_expired->queues[prio];
        _shed_d_d->rt_expired->queues[prio] = _shed_d_d->rt->queues[prio];
        _shed_d_d->rt->queues[prio] = tmp;
    }

    // se resetea el punto en el que estaba trabajando
    _shed_d_d->actual_prio = 0;
    _shed_d_d->count_reschedule = 0;
    
}

// Devuelve el siguiente PCB según la prioridad.
// Devuelve nulo en caso de que no haya ninguno
pcb_t * scheduler_get_next(sched_disp_data_t * _shed_d_d){
    pcb_t * _ret = NULL;
    
    // Mientras este dentro del rango de prioridades, va cogiendo de la cola de prioridades hasta que se queda sin ellas
    while ( (_shed_d_d->rt->bitmap_queue & (0x1 << _shed_d_d->actual_prio)) == 0 && _shed_d_d->actual_prio < _shed_d_d->rt->size)
        _shed_d_d->actual_prio++;
    if ( _shed_d_d->actual_prio < _shed_d_d->rt->size )
    {
        // Pillar de aqui
        ret = takeFromQueue(_shed_d_d->rt, _shed_d_d->actual_prio);
        if( ret == NULL)
        {
            fprintf(stderr, "Oh no \n");
            return NULL;
        }
        ret->q = (_shed_d_d->actual_prio + 1)*_shed_d_d->q_per_priority;
        ret->state = PCB_STATE_RUNNING;

    }
    else 
    {
        printf("Ha recorrido todas las colas y no hay más procesos listos\n");     
    }
    
    // Arbol
    /*else 
    {
        // luego esta el arbol, que esas mantienen el tiempo constante?
        // o que se haga porcentage de cpu
        ret = removeNode(tree->smallest_nice, tree);
        porcentaje = (float)prio_to_weight[ret->nice]/(float)total_weight;
        ret->q = (int) roundf(porcentaje*time_slice);
    }
    */
    return ret;

}


// Este scheduler reparte las tareas entre los otros schedulers
// Politicas de reparto de tareas nuevas:
// afinidad
// roundrobin
// el más ocioso
void schedulerPrincipal(){
    thread_t *hilo;

    int contador_max = paramStruct.n_cpu * paramStruct.n_core * paramStruct.n_thread;
    int contador;
    while (machine.idle_threads > 0 && (dequeing = ))
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
}

// Subrutina que se encarga de simular el desbloqueo de los procesos autobloqueados
void blocked_pcbs(sched_disp_data_t * _sched_d_d){
    list_pcb_t * list;
    l_node_t * p, p_;
    
    list = _sched_d_d->blocked_list;
    p = list->first;

    while ( p != NULL ) 
    { 
        p->elem->blocked--;
        // Comprueba que se tenga que desbloquear y que pueda añadirse a la cola de prioridades
        if ( p->elem->blocked <= 0 && (rt_addPCB(p->elem) != 'n') )
        {
            p->elem->state = PCB_STATE_IDLE;
            p_ = p;
            p = p->next;    // Avanzando antes de que lo pierda por quitarlo de la lista
            list_removeNode(list, p_);
        } 
        else p = p->next;
    }
    // Resta a los contadores del bloqueo y si termina intenta meterlo en la estructura rt
    
}

void *start_schedule(void *_args)
{
   
    int cpu, core, thread;
    int flag_found, thread_counter, contador_max;
    int count_queue;
    

   
    pcb_t *dequeing;
    sched_disp_data_t * sched_d_d;

    // TODO: init estructuras, pillar el asigned core


    while (1)
    {
        pthread_mutex_lock(&sched_mtx);
        sched_flag = 1;
        // esperando al timer, al tick
        while (sched_flag == 1)
            pthread_cond_wait(&sched_cond, &sched_mtx);

        printf("Dispatcher haciendo cosas\n");
        // Saca procesos terminados, bloqueados o los que se les han acabado el quantum por la planificación
        // A su vez, hace el amago de cambiar de estado e introduce en el procesador el proceso proporcionado por el scheduler
        dispatcher(sched_d_d);      
        printf("Dispatcher ha terminado de hacer cosas\n");

        printf("Simular el blocked time de los pcbs autobloqueados\n");
        // Simular el blocked time de los pcbs autobloqueados
        if (sched_d_d->blocked_list.nElem != 0)     blocked_pcbs(sched_d_d);
        printf("Terminando el blocked\n");


        // Reschedule,  recalcular parametros si se pasa X tiempo o se termina un ciclo en las colas activas y vuelven a las expiradas o caducadas
        if ( sched_d_d->count_reschedule >= sched_d_d->max_count_reschedule || sched_d_d->actual_prio >= sched_d_d->rt->size)
        {
            printf("Reschedule\n");
            reschedule(sched_d_d);
            printf("Reschedule finished \n");

        }
        sched_d_d->count_reschedule++;


        printf("Sacando de la cola de nuevos\n");
        // Sacar PCB de la cola de nuevos y meterlo en las colas
        count_queue = sched_d_d->new_q.nElem;
        while(count_queue > 0)
        {
            dequeing = dequeue(sched_d_d->new_q);
            rt_addPCB(sched_d_d->rt, dequeing);
            count_queue--;
        }
        printf("Terminando de sacar de la cola de nuevos\n");

        // Si esta dentro del arbol, recuperar el weight 
        // Arbol
        /*
        total_weight += prio_to_weight[_blocked_pcb->nice];
        */
    
        pthread_mutex_unlock(&sched_mtx);
    }
}