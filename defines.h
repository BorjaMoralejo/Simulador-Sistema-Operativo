/* defines.h
Este fichero contiene los defines del proyecto: Simulador de Sistema Operativo
*/
//mempool_t memPCBs;

#ifndef _DEFINES_H
#define _DEFINES_H 1

// Valores por defecto
#define DEFAULT_POOLSIZE 256
#define DEFAULT_QUEUESIZE 128
#define DEFAULT_CPU 1
#define DEFAULT_CORE 2
#define DEFAULT_THREAD 1

// Simulador.c
enum enum_threadName {CLOCK_TH, TIMER0_TH, TIMER1_TH, SCHEDULER_TH, PGEN_TH, NTHREADS};


// Timer
enum enum_timerFuncTypes {DISPATCHER_SCHEDULER_FUNC, PGENERATOR_FUNC};

#endif

