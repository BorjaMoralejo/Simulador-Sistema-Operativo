#ifndef _SCHEDULER_H
#define _SCHEDULER_H 1

#include <pthread.h>

#include "arbol.h"
#include "rt_struct.h"
#include "queue.h"
#include "estructuras.h"

void init_scheduler();
void * start_schedule(void * _args);
pcb_t * scheduler_get_next();

typedef struct scheduler_dispacher_data sched_disp_data_t;

typedef struct scheduler_dispacher_data{
   
    int id;
    int assigned_core, assigned_cpu;
    core_t * assigned_core;
    rt_struct_t * rt;
    rt_struct_t * rt_expired;
    list_pcb_t blocked_list;
    queue_pcb_t new_q;
    int count_reschedule, max_count_reschedule;
    int actual_prio;
    int q_per_priority = 10;
    // tree_t * tree;
    // int tree_base_q = 300;
    // int total_weight;
    // int time_slice = 20; 
}sched_disp_data_t;





#endif