
#ifndef _TLB_H
#define _TLB_H 1
#include "estructuras.h"

#define BIT_MASK_VALID 0x80000000

void clean_TLB(thread_t * _thread);
void add_to_TLB(thread_t * _thread, unsigned int _page, unsigned int _physical);

#endif