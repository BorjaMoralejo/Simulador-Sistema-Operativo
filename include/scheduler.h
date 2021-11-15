#ifndef _SCHEDULER_H
#define _SCHEDULER_H 1

#include <pthread.h>

#include "arbol.h"
#include "rt_struct.h"
#include "queue.h"
#include "list.h"
#include "estructuras.h"

#define POLITICA_ROUNDROBIN 0
#define POLITICA_AFINIDAD 1
#define POLITICA_AFINIDAD_FORZOSA 2
//Se iba a utilizar con un arbol de threads para esto, pero la estructura del arbol está sin temrinar
//#define POLITICA_OCIOSO 2 



typedef struct scheduler_dispacher_data sched_disp_data_t;

void init_scheduler(sched_disp_data_t ** _sched_arr);
void init_individual_scheduler(sched_disp_data_t * _sched_d_d);
void * start_schedule_master(void * _args);
void * start_schedule_slave(void * _args);


pcb_t * scheduler_get_next(sched_disp_data_t * _shed_d_d);


typedef struct scheduler_dispacher_data{
   
    int id;
    int core_id, cpu_id;
    core_t * assigned_core;
    rt_struct_t * rt;
    rt_struct_t * rt_expired;
    list_pcb_t blocked_list;
    queue_pcb_t new_q;
    int count_reschedule, max_count_reschedule;
    int actual_prio;
    int q_per_priority;
    // tree_t * tree;
    // int tree_base_q = 300;
    // int total_weight;
    // int time_slice = 20; 
}sched_disp_data_t;





#endif