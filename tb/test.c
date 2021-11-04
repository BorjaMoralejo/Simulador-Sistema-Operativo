#include "estructuras.h"
#include "globals.h"
#include "queue.h"
#include <stdio.h>

void test(int param_nElementosCola){

    pcb_t * pcb;
	for(int i = 0; i < param_nElementosCola;i++){
		pcb = getPCB();
		pcb->pid = i+1;
		enqueue(&q_pcb, pcb);
	}
	
	ll_node_int_t *p;
	p= memPCBs.cola_idx.first;
	while(p != NULL){
		printf("Indice %d \n",p->q_int);
		p = p-> sig;
	}


	pcb_t* testing;
	for(int i = 0; i < 25; i++){
		testing = dequeue(&q_pcb);
		putPCB(testing);
	}

	p= memPCBs.cola_idx.first;
	while(p != NULL){
		printf("Indice %d \n",p->q_int);
		p = p-> sig;
	}

	for(int i = 0; i < 25;i++){
		pcb = getPCB();
		pcb->pid = 100+i+1;
		enqueue(&q_pcb, pcb);
	}

	for(int i = 0; i < param_nElementosCola; i++){
		testing = dequeue(&q_pcb);
		putPCB(testing);
		printf("PID %d \n",testing->pid);		
	}
	p= memPCBs.cola_idx.first;
	while(p != NULL){
		printf("Indice %d \n",p->q_int);
		p = p-> sig;
	}


}