#include <stdlib.h>
#include <stdio.h>
#include "estructuras.h"
#include "list.h"

void init_list (list_pcb_t * _list, int _maxElem){
    _list->first = NULL;
    _list->last = NULL;
    _list->nElem = 0;
    _list->maxElem = _maxElem;
    _list->list_nodes = malloc( sizeof( l_node_t ) * _maxElem);
}

char list_addPCB (list_pcb_t * _list, pcb_t * _elem){
    if (_list->last == NULL) // Es el primero
    {
        _list->last = &_list->list_nodes[_list->nElem++];
        _list->first = _list->last;
        _list->last->next = NULL;
        _list->last->prev = NULL;
        
    }
    else 
    {
        if ( _list->nElem == _list->maxElem)
        {
            fprintf(stderr, "La lista está llena\n");
            return 1;
        }
        _list->last->next = &_list->list_nodes[_list->nElem++];
        _list->last->next->prev = _list->last;
        _list->last = _list->last->next;
        _list->last->next = NULL;
    }
    _list->last->elem = _elem;
    return 0;
}

void list_removeNode(list_pcb_t * _list, l_node_t * _target){


    if (_target == _list->last && _target == _list->first)  // Único elemento de la lista
    {
        _list->first = NULL;
        _list->last = NULL;
    }
    else if (_target == _list->last)                        // Último elemento de la lista
    {
        _list->last = _target->prev;
        _list->last->next = NULL;
    } 
    else if (_target == _list->first)                       // Primer elemento de la lista
    {
        _list->first = _target->next;
        _list->first->prev = NULL;
    }else                                                   // Cualquier otro nodo de la lista
    {
        _target->next->prev = _target->prev;
        _target->prev->next = _target->next;
    }

    _list->nElem--;
}

