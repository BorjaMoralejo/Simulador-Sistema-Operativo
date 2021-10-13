/* ------------------
 *  mainf.c
 * ----------------------------------------------
 *  Parte 1 de práctica de la asignatura SESO
 *	Autor: Borja Moralejo Tobajas
 *	Fecha: 1 de Octubre de 2021s
 * ---------------------------------------------- */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "defines.h"
#include "estructuras.h"
#include "queue.h"
#include "test.h"
#include <getopt.h>
#define PRINTF_EN 1

mempool_pcb_t memPCBs;
lkdList_int_t *cola_idx;
queue_pcb_t q_pcb;
machine_t machine;


// Parámetros default
int param_nElementosCola = 100;
int param_nPool= 256;
int n_cpu;
int n_core;
int n_thread;

void procesarParametros(int argc, char *argv[]);

int main(int argc, char *argv[]){

	

	// Tratar parámetros
	procesarParametros(argc, argv);

	// Tratamiento de errores


	// Inicializar estructuras de datos
	
	// 	Inicializar el almacenador de PBS
	inicializar_estructura(param_nPool);
	// 	Inicializar el indexador de PBS
	inicializar_linkedList_int(&memPCBs.cola_idx, param_nPool);
	// 	Inicializar colas del dispatcher
	inicializar_queue_pcb(&q_pcb, param_nElementosCola);
	
	

	test(param_nElementosCola);
	
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

int errorParametros(char *s) {
 fprintf(stderr,"***error*** %s\n",s);
 exit(-1);
} 
int getInt(char * _numero){
	char *fin;
	long value = strtol(_numero, &fin, 10); 
	if (fin == _numero || *fin != '\0' || errno == ERANGE){
		fprintf(stderr, "Error, dato introducido no es un número: %s",_numero);
		exit(3);
	}
}
void procesarParametros(int argc, char *argv[]){ 
 	int opt, longindex;
	opterr = 0;
 	struct option long_options[] = {
	{"poolsize", required_argument, 0, 'p' },
	{"queuesize", required_argument, 0, 'q' },
	{"n_cpu", required_argument, 0, 'c' },
	{"n_core", required_argument, 0, 'k' },
	{"n_thread", required_argument, 0, 't' },
	{"help", no_argument, 0, 'h' },
	{0, 0, 0, 0 }
	};

	longindex =0;
	while ((opt = getopt_long(argc, argv,"hc:r:", long_options, &longindex )) != -1) 
	{

		switch(opt) 
		{
		case 'p': /* poolsize */
			param_nPool = atoi(optarg);
			if(param_nPool <= 0) 
				errorParametros("Columnas: Fuera de rango");			

			break;
		case 'q': /* queuesize */
			param_nElementosCola = atoi(optarg);
			if(param_nElementosCola <= 0) 
				errorParametros("Filas: Fuera de rango");

			break;
		case 'c':
			n_cpu = atoi(optarg);
			if(n_cpu <= 0) 
				errorParametros("Filas: Fuera de rango");

		break;
		case 'k':
			n_core = atoi(optarg);
			if(n_core <= 0) 
				errorParametros("Filas: Fuera de rango");

		break;
		case 't':
			n_thread = atoi(optarg);
			if(n_thread <= 0) 
				errorParametros("Filas: Fuera de rango");

		break;
		case 'h': /* -h or --help */
		case '?':
			printf ("Uso %s [OPTIONS]\n", argv[0]);
			/*printf (" -c --column=NNN\t"
			"Ńº de columnas [%d]\n", COLUMN_DEFAULT);*/
			printf (" -h, --help\t\t"
			"Ayuda\n");
			/*printf (" -r --row=NNN\t\t"
			"Ńº de filas [%d]\n", ROW_DEFAULT);*/
			exit (2);
		default:
			errorParametros("Unknown argument option");
			exit(2);
		} // switch
	} // while
} // procesarParam