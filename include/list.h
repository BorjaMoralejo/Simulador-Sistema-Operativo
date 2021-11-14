#ifndef _LIST_H
#define _LIST_H 1


#include "estructuras.h"

// structs
typedef struct list_pcb list_pcb_t;
typedef struct list_node l_node_t;

typedef struct list_pcb{
    l_node_t * first;
    l_node_t * last;
    int nElem, maxElem;
    l_node_t * list_nodes;
} list_pcb_t;

// nodo
typedef struct list_node{
    l_node_t * next, * prev;
    pcb_t * elem;
} l_node_t;

// init
void init_list (list_pcb_t * _list, int _maxElem);
// añadir
char list_addPCB (list_pcb_t * _list, pcb_t * elem);
// quitar
void list_removeNode(list_pcb_t * _list, l_node_t * _target);


#endif