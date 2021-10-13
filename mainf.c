/* mainf.c
Parte 1 de práctica de la asignatura SESO
	Autor: Borja Moralejo Tobajas
	Fecha: 1 de Octubre de 2021


*************************************** */

#include <stdlib.h>
#include "defines.h"
//#include "estructuras.h"
#include "estructuras.c"



//mempool_pcb_t memPCBs;


int main(){

	int param_nElementosCola = 100;
	int param_nPool= 256;
	// Tratamiento de errores

	// Tratar parámetros
	
	//memPCBs = malloc(sizeof(mempool_pcb_t));
	// Declaraciones de variables
	mempool_pcb_t memPCBs;
	//mempool_pcb_t memPCB;
	//memPCBs = memPCB;
	machine_t machine;
	queue_pcb_t q_pcb;
	// Inicializar estructuras de datos
	
	// 	Inicializar el almacenador de PBS
	inicializar_estructura(&memPCBs, param_nPool);
	// 	Inicializar el indexador de PBS
	inicializar_queue_int(&memPCBs.cola_idx);
	// 	Inicializar colas del dispatcher
	inicializar_queue_pcb(&q_pcb, param_nElementosCola);
	
	
	pcb_t * pcb;
	for(int i = 0; i < param_nElementosCola+1;i++){
		pcb = getPCB(&memPCBs);
		pcb->pid = i+1;
		enqueue(&q_pcb, pcb);
	}

	pcb_t* testing;
	for(int i = 0; i < 25; i++)
		dequeue(&q_pcb);
	//printf("%ld\n", testing);
	//printf("Testing 1 %d \n",testing->pid);
  	//testing = dequeue(&q_pcb);
	//printf("Testing 2 %d \n",testing->pid);

	// TEST:
	// printear todos los elementos de la lista
	/*
	q_node_int_t *p;
	p= memPCBs.cola_idx.first;
	for(int i = 0; i < memPCBs.maxElem; i++)
	{
		printf("Indice :%d  es %d \n",i,p->q_int);
		p = p-> sig;
	}
	*/

//	enqueue();	
	
	// Inicializar elementos de sincronización

	// Lanzar hilos que implementaran los distintos subsistemas
	
	// Realizar la comunicación utilizando memoria compartida y los elementos de sincronización vistos en clase.
	//printf("1\n");
	free(q_pcb.malloc);
	//printf("2\n");
	free(memPCBs.cola_idx.malloc);
	
	free(memPCBs.malloc);
	//printf("3\n");
	//printf("4\n");
}
