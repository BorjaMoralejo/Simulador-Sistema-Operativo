#include "globals.h"
#include "estructuras.h"

//#include "defines.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//mempool_t memPCBs;


// Inicializando pool principal de PCBs
void inicializar_estructura(int _maxElem){
	memPCBs.nElem = 0;
	memPCBs.maxElem = _maxElem;
	memPCBs.malloc = malloc(sizeof(pcb_t)*_maxElem);
}

void inicializar_linkedList_int(lkdList_int_t *_cola, int _maxElem){
	#ifdef PRINTF_EN
	printf("Inicializando cola de integers\n");
	#endif
	int i;
	ll_node_int_t *arr_nodos;

	
	// crear _maxElem elementos de nodos de maxElem y asignar primero y último
	_cola->nElem = _maxElem;
	_cola->maxElem = _maxElem;
	arr_nodos = _cola->malloc = malloc(sizeof(ll_node_int_t)*_cola->maxElem);
	_cola->first = &arr_nodos[0];
	_cola->last = &arr_nodos[_cola->maxElem-1];
	
	// Linkear los nodos y asignar valores
	for( i = 0; i < _cola->maxElem-1; i++)
	{
		arr_nodos[i].sig = &arr_nodos[i+1];
		arr_nodos[i].q_int = i;
	}
	
	// Asignando el último elemento
	arr_nodos[_cola->maxElem - 1].q_int = _maxElem-1;
	arr_nodos[_cola->maxElem-1].sig = NULL;
}



pcb_t * getPCB(){
	pcb_t *_ret;
	lkdList_int_t *_cola_idx = &memPCBs.cola_idx;

	_cola_idx->nElem--;
	// Coge índice libre de mempool y avanza la lista
	ll_node_int_t * _indice = _cola_idx->first;
        _cola_idx->first = _indice->sig;
	// Coge el pcb que le toca
	_ret = &memPCBs.malloc[_indice->q_int];
	_ret->indice = _indice;
	_indice->sig = NULL;
	return _ret;
}

void putPCB(pcb_t * _elem){
	ll_node_int_t * _ultimo = memPCBs.cola_idx.last;
	memPCBs.cola_idx.last = _elem->indice;
	_ultimo->sig = _elem->indice;
	memPCBs.nElem++;
}