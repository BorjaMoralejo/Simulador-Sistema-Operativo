
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
#define mask_decode_r1      0x00F00000
#define mask_decode_r1      0x000F0000
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


// definir como sacar los datos, mascaras
void fetch(thread_t * _thread){
    int * instruccion;
    // buscar la info en la posicion que sea
    return instruccion;
}

void decode(thread_t * _thread, int * _ri){
    // decode, enviar a siguiente operacion que toque

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