#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "scheduler.h"
#include "dispatcher.h"
#include "globals.h"
#include "defines.h"
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

// Inicializar las estructuras de cada uno de los schedulers.
// Cada scheduler llama a esta subrutina y se crea un "objeto" de scheduler de esta forma.
void init_scheduler(sched_disp_data_t ** _sched_arr)
{
    int i, n_of_slaves = paramStruct.n_core*paramStruct.n_cpu;

    pthread_mutex_init(&sched_mtx, NULL);
    pthread_cond_init(&sched_cond, NULL);

    pthread_mutex_init(&multi_sched_mtx, NULL);
    pthread_cond_init(&multi_sched_cond, NULL);
    pthread_cond_init(&br_multi_sched_cond, NULL);
    sched_done = 0;

    *_sched_arr = malloc( sizeof (sched_disp_data_t) * n_of_slaves );

    // Inicializar datos de los schedulers esclavos
    for ( i = 0; i < n_of_slaves; i++)
    {
       init_individual_scheduler ( &(*_sched_arr)[i] );
    }

}

void init_individual_scheduler(sched_disp_data_t * _sched_d_d){
    printf("\n\n--------\n Inicializando scheduler %d\n", sched_id);
    // Asignando variables para saber que scheduler es
    _sched_d_d->id = sched_id++;
    _sched_d_d->actual_prio = 0;

    // Asignando core a scheduler
    int cpu_id = _sched_d_d->id / paramStruct.n_core;
    int core_id = _sched_d_d->id % paramStruct.n_core;
    _sched_d_d->assigned_core = &machine.cpus[cpu_id].cores[core_id];
    _sched_d_d->core_id = core_id;
    _sched_d_d->count_reschedule = 0;
    _sched_d_d->cpu_id = cpu_id;
    _sched_d_d->max_count_reschedule = paramStruct.freq_reschedule;

    // init colas de bloqueado y procesos nuevos
    init_list( &_sched_d_d->blocked_list, paramStruct.blocked_list_size );    
   // printf("4a ");

    inicializar_queue_pcb(&_sched_d_d->new_q, paramStruct.nElementosCola);
    //printf("5a ");

    _sched_d_d->q_per_priority = paramStruct.quantum_per_prio;

    // init rt y rt expired
    _sched_d_d->rt = malloc(sizeof(rt_struct_t));
    init_rt_struct(_sched_d_d->rt, paramStruct.max_prio);
    //printf("6a ");

    _sched_d_d->rt_expired = malloc(sizeof(rt_struct_t));
    init_rt_struct(_sched_d_d->rt_expired, paramStruct.max_prio);
    //printf("7a\n");

}


// Cada X ciclos se recalculan los parametros del scheduler y está es la función que lo hace
void reschedule(sched_disp_data_t * _shed_d_d){
    pcb_t * buf;
    queue_pcb_t tmp;
    rt_struct_t * swap_rt;

    int size = _shed_d_d->rt->size;
    long int swap;
    // los procesos que estaban en proceso se dejan porque van a ir a la cola expirado igualmente
    printf("estado del bitmap: %08lx  |  %08lx\n",_shed_d_d->rt->bitmap_queue, _shed_d_d->rt_expired->bitmap_queue);

    int prio = 0;
    // PASAR DE EXPIRED A NO EXPIRED
    while (prio < size)
    {
        while ((_shed_d_d->rt_expired->bitmap_queue & (0x1 << prio)) != 0 && (buf = takeFromQueue(_shed_d_d->rt_expired, prio)) != NULL)
        {
            if ( rt_addPCB(_shed_d_d->rt, buf) == 1 ) {
                fprintf(stderr,"Oh no, la cola se ha llenado, devolviendo a memoria principal...\n");     
                putPCB(buf);
            }
            else 
            {
                printf("PID=%d se ha movido a la estructura activa!\n", buf->pid);
            }
        }
        prio++;
    }
    printf("estado del bitmap: %08lx  |  %08lx\n",_shed_d_d->rt->bitmap_queue, _shed_d_d->rt_expired->bitmap_queue);

    // se resetea el punto en el que estaba trabajando
    _shed_d_d->actual_prio = 0;
    _shed_d_d->count_reschedule = 0;
}

// Devuelve el siguiente PCB según la prioridad.
// Devuelve nulo en caso de que no haya ninguno
pcb_t * scheduler_get_next(sched_disp_data_t * _sched_d_d){
    pcb_t * ret = NULL;
    // Mientras este dentro del rango de prioridades, va cogiendo de la cola de prioridades hasta que se queda sin ellas
    while ( (_sched_d_d->rt->bitmap_queue & (0x1 << _sched_d_d->actual_prio)) == 0 && _sched_d_d->actual_prio < _sched_d_d->rt->size)
        _sched_d_d->actual_prio++;

    //printf("Prio %d %d %ld \n",_sched_d_d->actual_prio, _sched_d_d->rt->queues[_sched_d_d->actual_prio].nElem,_sched_d_d->rt->bitmap_queue);

    if ( _sched_d_d->actual_prio < _sched_d_d->rt->size )
    {
        // Pillar de aqui
        ret = takeFromQueue(_sched_d_d->rt, _sched_d_d->actual_prio);
        if( ret == NULL)
        {
            fprintf(stderr, "Oh no %d\n", _sched_d_d->actual_prio );
            return NULL;
        }
        ret->q = (_sched_d_d->actual_prio + 1)*_sched_d_d->q_per_priority;
        ret->state = PCB_STATE_RUNNING;

        // Seguir mirando para ver si era el ultimo
        if (_sched_d_d->rt->queues[_sched_d_d->actual_prio].nElem == 0)
            while ( (_sched_d_d->rt->bitmap_queue & (0x1 << _sched_d_d->actual_prio)) == 0 && _sched_d_d->actual_prio < _sched_d_d->rt->size)
                _sched_d_d->actual_prio++;
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

/*cpu = contador/(paramStruct.n_core*paramStruct.n_thread);
core = (contador%(paramStruct.n_core*paramStruct.n_thread))/(paramStruct.n_thread);
thread = contador%paramStruct.n_thread;*/
            


// Subrutina que se encarga de simular el desbloqueo de los procesos autobloqueados
void blocked_pcbs(sched_disp_data_t * _sched_d_d){
    list_pcb_t * list;
    l_node_t * p, * p_;
    
    list = &_sched_d_d->blocked_list;
    p = list->first;
    printf("Hay %d elementos bloqueados!\n", list->nElem);
    while ( p != NULL ) 
    { 
        p->elem->blocked--;
        printf("PID=%d BLOCKED=%ld\n", p->elem->pid, p->elem->blocked);
        // Comprueba que se tenga que desbloquear y que pueda añadirse a la cola de prioridades
        if ( p->elem->blocked <= 0 && (rt_addPCB(_sched_d_d->rt_expired , p->elem) != 1) )
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

// Este scheduler reparte las tareas entre los otros schedulers
// Politicas de reparto de tareas nuevas:
// afinidad
// roundrobin
// el más ocioso (WIP)
void *start_schedule_master(void *_args){
    // Array de "objetos" de schedulers esclavos
    sched_disp_data_t * sched_arr = (sched_disp_data_t *) _args;

    // Variables utilizadas como atributos
    int round_robin;
    int politica= paramStruct.politica_scheduler_master;
    int contador_max = paramStruct.n_cpu * paramStruct.n_core;

    // Variables auxiliares
    thread_t *hilo;
    int contador,found;
    int cpu, core, thread;
    pcb_t * dequeing;
    int more = 1;

    while (1)
    {
        pthread_mutex_lock(&sched_mtx);
        sched_flag = 1;

        // esperando al timer, al tick
        while (sched_flag == 1)
            pthread_cond_wait(&sched_cond, &sched_mtx);

        more = 1;
        printf("-------------------- Scheduler maestro -------------------- \n");

        printf("Comprobando que haya procesos nuevos en la cola...\n");
        if(q_pcb.nElem != 0)
        {
            printf("Repartiendo PCBs nuevos! Hay %d en la cola maestra\n", q_pcb.nElem);
        }
        // Repartiendo tareas
        while (more == 1 && (dequeing = dequeue(&q_pcb)))
        {   
           // printf("\n\tBuscando pcb de la cola de nuevos para %d\n", round_robin);
            found = 0;
            contador = 0;
            if(politica == POLITICA_AFINIDAD)
            {
                round_robin = dequeing->affinity;
                printf("Afinidad del hilo %d !\n", dequeing->affinity);
            }
            if (round_robin >= contador_max || round_robin < 0)
                round_robin = 0;
                
            while(found == 0 && contador < contador_max)
            {
               // printf("\tCiclo de busqueda %d y rr = %d\n", contador, round_robin);

                if ( enqueue (&sched_arr[round_robin].new_q, dequeing) == 0)
                {
                    found = 1;
                    printf("\tColocado el proceso nuevo PID:%d en la cola de PCBs nuevos del scheduler %d\n", dequeing->pid ,round_robin);
                }

                round_robin++;
                if (round_robin >= contador_max)
                    round_robin = 0;

                contador++;
            }
            
            // No hay huecos
            if ( found == 0)
            {
                printf("\tNo hay huecos en las colas de procesos nuevos de los esclavos\n");
                enqueue ( &q_pcb, dequeing );
                more = 0;
            }
        } // while

        printf("\n-------------------- Schedulers esclavos -------------------- \n");

        // Dar via libre a los schedulers esclavos
        pthread_mutex_lock(&multi_sched_mtx);

        sched_done = 0;

        // notificar a los esclavos
        pthread_cond_broadcast(&br_multi_sched_cond);

        // Una vez los esclavos terminan, continuar
        while(sched_done < contador_max){
            pthread_cond_wait(&multi_sched_cond, &multi_sched_mtx);
        }

        sched_done = 0;
        
        pthread_mutex_unlock(&multi_sched_mtx);

        // Terminar y devolver control al timer
        pthread_mutex_unlock(&sched_mtx);
    }
}

// Bucle principal de los schedulers esclavos
void *start_schedule_slave(void *_args)
{

    // Variables que se utilizan como atributos del "objeto"
    //sched_disp_data_t sched_sched;
    sched_disp_data_t * sched_d_d = (sched_disp_data_t *) _args;

    // Variables auxiliares
    int count_queue;
    pcb_t *dequeing;
    
    //init_individual_scheduler(sched_d_d);

    pthread_mutex_lock(&multi_sched_mtx);
    printf("Pasando lock Scheduler Slave %d\n", sched_d_d->id);
    pthread_mutex_unlock(&multi_sched_mtx);
   // while(sched_done != 0);

    while(1)
    {
        pthread_cond_wait(&br_multi_sched_cond, &multi_sched_mtx);

        printf("------------------------- Scheduler Dispatcher con id %d ------------------------\n", sched_d_d->id);
        // Sacar PCB de la cola de nuevos y meterlo en las colas
        count_queue = sched_d_d->new_q.nElem;
        if (count_queue != 0)
        {
            printf("Sacando de la cola de nuevos %d elementos!\n", count_queue);
            while(count_queue > 0)
            {
                dequeing = dequeue(&sched_d_d->new_q);

                // Añadiendo y comprobando que la cola de prioridades no este llena
                if(rt_addPCB (sched_d_d->rt_expired, dequeing) == 1) 
                {
                        enqueue ( &sched_d_d->new_q, dequeing);
                        count_queue = 0;
                }else 
                {
                    printf("Colocado en la cola a PID=%5d\n", dequeing->pid);
                }
                
                count_queue--;
            }
            printf("Terminando de sacar de la cola de nuevos, estado del bitmap: %08lx  |  %08lx\n",sched_d_d->rt->bitmap_queue, sched_d_d->rt_expired->bitmap_queue);
            printf("Total en la cola de nuevos:%d\n", sched_d_d->new_q.nElem);
            printf("Total en cola del rt:%d\n", sched_d_d->rt->queues[0].nElem);

        }   

        // Saca procesos terminados, bloqueados o los que se les han acabado el quantum por la planificación
        // A su vez, hace el amago de cambiar de estado e introduce en el procesador el proceso proporcionado por el scheduler
        dispatcher(sched_d_d);      

        // Simular el blocked time de los pcbs autobloqueados
        if (sched_d_d->blocked_list.nElem != 0)
        {
            printf("Simular el blocked time de los pcbs autobloqueados\n");
            blocked_pcbs(sched_d_d);
            printf("Terminando el blocked\n");
        } 


        // Reschedule,  recalcular parametros si se pasa X tiempo o se termina un ciclo en las colas activas y vuelven a las expiradas o caducadas
        if ( sched_d_d->count_reschedule >= sched_d_d->max_count_reschedule || sched_d_d->actual_prio >= sched_d_d->rt->size)
        {
            if (sched_d_d->actual_prio >= sched_d_d->rt->size)
                printf("Reschedule forzoso\n");
            else
                printf("Reschedule por tiempo\n");

            reschedule(sched_d_d);

        }
        sched_d_d->count_reschedule++;


        // Si esta dentro del arbol, recuperar el weight 
        // Arbol
        /*
        total_weight += prio_to_weight[_blocked_pcb->nice];
        */

        sched_done++;
        pthread_cond_signal(&multi_sched_cond);
        printf("-------------------------------------------------\n");


    }      
}