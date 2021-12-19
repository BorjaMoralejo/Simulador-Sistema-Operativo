#ifndef _MACHINE_H
#define _MACHINE_H 1

#include "estructuras.h"
void init_thread(thread_t * _thread);
void clean_TLB(thread_t * _thread);

#endif