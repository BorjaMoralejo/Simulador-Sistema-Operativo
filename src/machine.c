
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
#define mask_absolute_dir   0x00FFFFFF

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



enum operation{ADD, ST, LD, EXIT };

void init_thread(thread_t * _thread){
    // inicializando vector de registros
    thread_l->rn = malloc(sizeof(int)*16);
    
    // asignando PTBR
    thread_l->PTBR;

    // generando mmu
    _thread->mmu;
}

void do_command(){
    thread_t *t;
    int * ri_dir; // ?????
    ri = fetch(t)
    decode ( t, ri);


}

// definir como sacar los datos, mascaras
int * fetch(thread_t * _thread){
    int * instruccion;
    // buscar la info en la posicion que sea
    _thread->pc;

    // coger la direccion virtual del pc?
    (*_thread->enProceso)->mm.code_p;

    // coger la instruccion y meterla en RI

    return instruccion;
}

void decode(thread_t * _thread, int * _ri){
    // decode, enviar a siguiente operacion que toque
    (*_thread->enProceso)->mm;

    int op = ri & mask_decode_op;
    switch(){
        case ld:
            printf("LD\n");
            break;
        case st:
            printf"ST\n");
            break;
        case add:
            printf("ADD\n");
            break;
        case exit:
            printf("EXIT\n");
            break;
        default:
            printf("Instrucción no valida");
            break;
    }

}

void load_op(thread_t * _thread, int _instruccion, int ** _ra, int ** _rb){
    // cargar operandos accediendo a memoria
    
}

void operate(thread_t * _thread, enum operation _op, int *_val1, int *_val2){
    switch (_op)
    {
    case ADD:
        break;
    case ST:
        break;
    case LD:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}


void write_results(thread_t * _thread, int *_rv, int ** _rd){
    // guardar resultado en el registro
}


void get_physical(thread_t *_thread, int _base, int _desplazamiento){
    // pillar mmu
    // mirar tlb en mmu y ver si existe la traduccion instantanea
    // hit or miss
    // i guess they never miss
    // hit
    // pues ya se tiene la direccion fisica
    // miss
    // mirar en la ptbr a que direccion apunta
}