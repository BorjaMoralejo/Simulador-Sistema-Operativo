
#include "estructuras.h"
// mmu
// tlb
// traduccion de instrucciones a acciones
//- [ ] Implementacion de exit
//			- [ ] Fetch 
//			- [ ] Decodificar ld, st, add, exit
//			- [ ] Busqueda de operandos (carga y guardado) st, add y ld
//			- [ ] Operar (exit saca del procesador)
//			- [ ] Escribir resultados ld y add en registros y st en memoria


#define mask_decode_op      0xF0000000
#define mask_decode_r1      0x0F000000
#define mask_decode_r2      0x00F00000
#define mask_decode_r3      0x000F0000
#define mask_decode_dir   0x00FFFFFF

#define ld      0
#define st      1
#define add     2
#define sub     3
#define mul     4
#define div     5
#define and     6
#define or      7
#define xor     8
#define mov     9
#define cmp     10
#define b       11
#define beq     12
#define bgt     13
#define blt     14
#define exit    15


extern unsigned char *ptbr_p;   // de Physical.c

enum operation{ADD, ST, LD, EXIT };

void init_thread(thread_t * _thread){
    int i;

    // inicializando vector de registros
    thread_l->rn = malloc(sizeof(int)*16);
    
    // asignando PTBR
    thread_l->PTBR = ptbr_p;

    // generando mmu
    _thread->mmu = malloc(sizeof(mmu_t));

    _thread->mmu->max_entradas = 32;
    _thread->mmu->entradas = malloc(sizeof(entrada_tlb_t) * _thread->mmu->max_entradas);

    for ( i = 0; i < _thread->mmu->max_entradas; i++ )
    {
        _thread->mmu->entradas[i].pagina = 0xFFFF;
    }
}

int * get_physical(thread_t *_thread, int _virtual){
    int i;
    char found = 0;
    unsigned dir_traducida = NULL;
    // pillar mmu

    // https://egela.ehu.eus/pluginfile.php/4977224/mod_resource/content/10/OS%20-%20Tema%203-1%20Sistema%20de%20gesti%C3%B3n%20de%20memoria.pdf
    // Pillar numero de pagina y offset

    // REHACER lo de tlb, hay que buscar usando el numero de pagina y traducirlo a numero de marco
    unsigned int pagina = _virtual & 0xFFFF00;
    unsigned int offset = _virtual & 0x0000FF;

    // Comprobar que la pagina sea del proceso?
    

    // mirar tlb en mmu y ver si existe la traduccion instantanea
    while (found == 0 && i < _thread->mmu.n_entradas)
    {
        // Si encuentra el numero de pagina en el tlb, se asigna el valor del TLB
        if(_thread->mmu.entradas[i].pagina == pagina)
        {
            dir_traducida = _thread->mmu.entradas[i].marco;
            found = 1;
        }
        i++;
    }
    
    // miss, deberia de bloquearse el programa (eso esta implementado, solo que esta random, esta en el clock)
    // De todos modos, busca la respuesta aqui para que al desbloquearse lo unico que tenga que hacer sera repetir esto
    if(found == 0)
    {
        i = 0;
        // mirar en la ptbr a que direccion apunta
        // Hacer while en PTBR hasta encontrar la direccion
        while ( i < 65535)
        {
            // dir_traducida la trae de la tabla de paginas, desplazamiento debido a tamaño de palabra
            if ( i == pagina )
            {
                dir_traducida = _thread->PTBR[i];
            }        
        }
        // ponerla en la tlb? cuando se desbloquee
        // Al ser una simulación limitada, se asigna el valor de la tlb al desbloquearse 
        // para simular la nueva entrada de la TLB.
        
    }

    dir_traducida |= offset;

    return dir_traducida;
}

/*
Hace el comando que le toca al thread.
Es posible que el comando sea EXIT y deba salir de la CPU.
*/ 
void do_command(thread_t * _thread){
    
    int * ri_dir; // ?????

    // Hacer esto con funciones "virtuales" para tener un puntero que va a ir cambiado y de esta
    // forma poder ir avanzando de operacion por turno
    // Mejor no por las posibles dependencias que se pueden crear

    
    ri = fetch(_thread)
    decode  ( _thread, ri);
    load_op ();
    operate ();
    write_results();
}

/*
Coge la instruccion de memoria y la pone en RI
*/
int * fetch(thread_t * _thread){
    int * instruccion;
    // buscar la info en la posicion que sea
    _thread->pc;

    // coger la direccion virtual del pc?
    (*_thread->enProceso)->mm.code_p;

    // coger la instruccion y meterla en RI

    return instruccion;
}

/*
Decodifica el registro RI para saber que operación debe hacer
*/
void decode(thread_t * _thread, int * _ri){
    // decode, enviar a siguiente operacion que toque
    (*_thread->enProceso)->mm;
    enum operation op_enum;
    int op = ri & mask_decode_op;
    switch(){
        case ld:
            printf("LD\n");
            op_enum = LD;
            break;
        case st:
            printf"ST\n");
            op_enum = ST;
            break;
        case add:
            printf("ADD\n");
            op_enum = ADD;
            break;
        case exit:
            printf("EXIT\n");
            op_enum = EXIT;
            break;
        default:
            printf("Instrucción no valida\n");
            break;
    }
    // Siguiente es load_op
}

/*
Según la operación debe cargar los datos de RI de una forma u otra. 
Para ello existen las mascaras mask_decode_(r1, r2, r3, dir)
*/
void load_op(thread_t * _thread, enum operation _op, int instruccion, int ** _ra, int ** _rb){
    // cargar operandos accediendo a los registros
    
}

/*
Debe hacer la operación que se ha descodificado. Esas operaciones pueden ser: ADD, ST, LD o EXIT
*/
void operate(thread_t * _thread, enum operation _op, int *_val1, int *_val2){
    int v;
    switch (_op)
    {
    case ADD:
        v = (*_val1) + (*_val2);
        break;
    case ST:
        // Cambiar el valor en memoria
        v = (*_val1);
        break;
    case LD:
        // Coger el valor desde la memoria
        v = (*_val2);
        break;
    case EXIT:
        // Se debe parar la ejecucion, el programa ha terminado
        break;
    default:
        printf("No se como he acabado aquí\n");
        break;
    }
}

/*
Guarda los resultados de las operaciones en sus registros correspondientes
*/
void write_results(thread_t * _thread, int *_rv, int ** _rd){
    // guardar resultado en el registro
    // por ejemplo, el resultado de la suma o el load
    // el store tiene que cambiar la memoria pero en OP
}
