#ifndef _LOADER_H
#define _LOADER_H 1

void init_loader();
void * start_loader(void * _args);

void unload_program(pcb_t * _pcb);

#endif