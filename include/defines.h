/* defines.h
Este fichero contiene los defines del proyecto: Simulador de Sistema Operativo
*/
#ifndef _DEFINES_H
#define _DEFINES_H 1

// Valores por defecto
#define DEFAULT_POOLSIZE        256
#define DEFAULT_QUEUESIZE       128
#define DEFAULT_CLOCK_MANUAL    0
#define DEFAULT_CLOCK_RETARDADO 0
#define DEFAULT_FREQ_CLOCK_MS   1000
#define DEFAULT_FREQ_DISP_SCHED 10
#define DEFAULT_FREQ_PGEN       12
#define DEFAULT_TTL_BASE        5
#define DEFAULT_TTL_MAX         42
#define DEFAULT_CPU             1
#define DEFAULT_CORE            2
#define DEFAULT_THREAD          1

#define DEFAULT_BLOCKED_LIST_SIZE   30
#define DEFAULT_BLOCK_CHANCE        800
#define DEFAULT_MAX_BLOCKED_TIME    30
#define DEFAULT_RANDOM_PRIORITY     -1
#define DEFAULT_RANDOM_AFFINITY     0
#define DEFAULT_FREQ_RESCHED        100
#define DEFAULT_Q_PER_PRIORITY      10
#define DEFAULT_MAX_PRIO            30
#define DEFAULT_PCB_GENERATED       1
#define DEFAULT_MASTER_SCHEDULER_BEHAVIOUR 0

#define DEFAULT_N_OF_PROGRAMS   1
#define DEFAULT_NAME            "prog"
#define DEFAULT_START_NUMBER    0

// Simulador.c
enum enum_threadName {CLOCK_TH, TIMER0_TH, TIMER1_TH, SCHEDULER_TH, PGEN_TH, NTHREADS};

// Timer
enum enum_timerFuncTypes {DISPATCHER_SCHEDULER_FUNC, PGENERATOR_FUNC, LOADER_FUNC};

#endif

