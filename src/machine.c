
#include "estructuras.h"
#include "tlb.h"
#include "stdlib.h"
#include "stdio.h"
#include "physical.h"

#define mask_decode_op      0xF0000000
#define mask_decode_r1      0x0F000000
#define mask_decode_r2      0x00F00000
#define mask_decode_r3      0x000F0000
#define mask_decode_dir     0x00FFFFFF

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

extern unsigned char *memoria;


enum operation{ADD, ST, LD, EXIT };

int fetch (thread_t * _thread);
void decode (thread_t * _thread, enum operation *_op);
void load_op (thread_t * _thread, enum operation _op, int * _ra, int * _rb, int * _rd);
int operate (thread_t * _thread, enum operation _op, int _val1, int _val2, int *_res, int * val);
void write_results (thread_t * _thread, int *_res, int * _rd);

void init_thread(thread_t * _thread){
    int i;

    // inicializando vector de registros
    _thread->rn = malloc(sizeof(int)*16);
    
    // asignando PTBR
    _thread->PTBR = NULL;

    // generando mmu
    _thread->mmu = malloc(sizeof(mmu_t));

    _thread->mmu->max_entradas = 32;
    _thread->mmu->entradas = malloc(sizeof(entrada_tlb_t) * _thread->mmu->max_entradas);

    //for ( i = 0; i < _thread->mmu->max_entradas; i++ )
    //{
      //  _thread->mmu->entradas[i].pagina;
    //}
}

/*
Consigue la dirección física con la dirección virtual de un proceso.
Si ocurre page fault, devuelve 1.
Si no estába en la TLB, devuelve -1 (y la dirección física correcta).
Correcto funcionamiento, devuelve 0.
*/
int get_physical(thread_t *_thread, int _virtual, unsigned int *_physical){
    int i;
    int ret = 0;
    char found = 0;
    unsigned dir_traducida = 0;
    int dir_op;
    // pillar mmu

    // https://egela.ehu.eus/pluginfile.php/4977224/mod_resource/content/10/OS%20-%20Tema%203-1%20Sistema%20de%20gesti%C3%B3n%20de%20memoria.pdf
    // Pillar numero de pagina y offset

// esto esta mal, hay que rehacerlo, la PTBR no deberia de ser como esta hecha lmao

    // REHACER lo de tlb, hay que buscar usando el numero de pagina y traducirlo a numero de marco
    unsigned int pagina = _virtual & 0xFFFF00;
    unsigned int offset = _virtual & 0x000000FF;
    printf("M_1\n");
    // Comprobar que la pagina sea del proceso?
    if((*_thread->enProceso)->mm.code_p > _virtual || (*_thread->enProceso)->mm.end_p < _virtual)
    {
        // Page fault
        return -1;
    }
    printf("M_2: Virtual %u Offset: %d \n", _virtual, offset);
    // mirar tlb en mmu y ver si existe la traduccion instantanea
    i = 0;
    while (found == 0 && i < _thread->mmu->max_entradas)
    {
        // Si encuentra el numero de pagina en el tlb, se asigna el valor del TLB
        if((_thread->mmu->entradas[i].pagina & BIT_MASK_VALID) == BIT_MASK_VALID &&  (_thread->mmu->entradas[i].pagina & 0x00FFFFFF) == pagina)
        {
            dir_traducida = _thread->mmu->entradas[i].marco;
            found = 1;
        }
        i++;
    }
    

    
    printf("M_3\n");
    // miss, deberia de bloquearse el programa (no tiene cache) (eso esta implementado, solo que esta random, esta en el clock)
    // De todos modos, busca la respuesta aqui para que al desbloquearse lo unico que tenga que hacer sera repetir esto
    if(found == 0)
    {
        // Al ser una simulación limitada, se asigna el valor de la tlb ahora 
        dir_op = (pagina - (*_thread->enProceso)->mm.code_p) / PAGE_SIZE;
        dir_traducida = _thread->PTBR[dir_op];        
        // Si esta llena se va a ir sobreescribiendo 
        printf("M_4: %u dir op %u\n", dir_traducida, dir_op);
        add_to_TLB(_thread, pagina, dir_traducida); 
        printf("M_5: \n");
        ret = 1;
    }

    dir_traducida += offset;

    (*_physical) = dir_traducida;
    return ret;
}




/*
Hace el comando que le toca al thread.
Es posible que el comando sea EXIT y deba salir de la CPU.
*/ 
int do_command(thread_t * _thread){
    
    int * ri_dir; // usar el pc para conseguir este valor
    int ra, rb, rd, rs;
    enum operation op;
    int ret_get_physical = 2;
    int next_action;

    // Hacer esto con funciones "virtuales" para tener un puntero que va a ir cambiado y de esta
    // forma poder ir avanzando de operacion por turno
    // Mejor no por las posibles dependencias que se pueden crear

    // B D L A E

    // BUSQUEDA
    printf("Fetch\n\n");
    next_action = fetch(_thread);
    if (next_action == -1) // Page fault
        return -2;
    printf("Decode\n\n");
    // DECODIFICACIÓN
    decode(_thread, &op);

    // CARGA DE DATOS
    // Se tiene que obtener los indices de los registros
    printf("load_op\n\n");
    load_op(_thread, op, &ra, &rb, &rd);


    // ACCIÓN (?)
    printf("operate\n\n");
    if (operate (_thread, op, ra, rb, &rs, &ret_get_physical) == -1)
    {
	    return -1;    // Exit
    }
    if (ret_get_physical == -1) // Page fault
        return -2;


    // ESCRITURA
    printf("write\n\n");
    write_results(_thread, &rs, &rd);

    if(next_action == 1 || ret_get_physical == 1)
        return 1;
    else 
        return 0;
}

/*
Coge la instruccion de memoria y la pone en RI.
Devulve si ha ocurrido: pagefault(1), no estaba en la tlb(-1) o correcto(0).
*/
int fetch(thread_t * _thread){
    int * instruccion;
    int virtual_dir;
    int physical_dir;
    int val;
    
    // coger la direccion virtual del pc
    virtual_dir = _thread->pc;
    _thread->pc += 4;
    // coger la instruccion y meterla en RI
    printf("PC %08X\n", _thread->pc);
	val = get_physical(_thread, virtual_dir, &physical_dir);
    printf("Physical %u\n", physical_dir);
    _thread->ri = get_at_dir(physical_dir); // Acceder a physical_dir
    printf("ri = %X\n", _thread->ri);
    return val;
}

/*
Decodifica el registro RI para saber que operación debe hacer
*/
void decode(thread_t * _thread, enum operation *_op){
    // decode, enviar a siguiente operacion que toque
    (*_thread->enProceso)->mm;
    enum operation op_enum;
    int op = (_thread->ri & mask_decode_op) >> 28;
    printf("Operación: %d\n", op);
    // Asignando la operacion
    switch(op){
        case ld:
            printf("LD\n");
            (*_op) = LD;
            break;
        case st:
            printf("ST\n");
            (*_op) = ST;
            break;
        case add:
            printf("ADD\n");
            (*_op) = ADD;
            break;
        case exit:
            printf("EXIT\n");
            (*_op) = EXIT;
            break;
        default:
            printf("Instrucción no valida\n");
            break;
    }
}

/*
Según la operación debe cargar los datos de RI de una forma u otra. 
Para ello existen las mascaras mask_decode_(r1, r2, r3, dir)
*/
void load_op(thread_t * _thread, enum operation _op, int * _ra, int * _rb, int * _rd){
    // cargar operandos accediendo a los registros
    switch(_op){
case ADD:  // Debe cargar rd, r1 y r2
	// Pasar de indice a valor
	(*_ra) = (_thread->ri & mask_decode_r2) >> 20;
	(*_rb) = (_thread->ri & mask_decode_r3) >> 16;
	(*_rd) = (_thread->ri & mask_decode_r1) >> 24;
    printf("Cargando operandos de ADD: rd %d,rf1 %d, rf2 %d\n", (*_rd), (*_ra), (*_rb));
	break;
case ST:   // Debe cargar el indice de ra             revisarlo con el enunciado
	(*_ra) = (_thread->ri & mask_decode_dir);
    (*_rb) = (_thread->ri & mask_decode_r1) >> 24;
    printf("Cargando operandos de ST: rd %d,ra %d\n", (*_rd), (*_ra));
	break;
case LD:   // Debe cargar el indice de rd
	(*_rd) = (_thread->ri & mask_decode_r1) >> 24;
    (*_ra) = (_thread->ri & mask_decode_dir);
    printf("Cargando operandos de LD: rd %d,ra %08X\n", (*_rd), (*_ra));
	break;
case EXIT: // No hace nada, no tiene registros destino ni origen
	printf("LD exit\n");
	break;
default:
	printf("Esto no debería mostrarse en pantalla\n");
	break;
    }
}

/*
Debe hacer la operación que se ha descodificado. Esas operaciones pueden ser: ADD, ST, LD o EXIT
El int que devuelve se utiliza para saber si es EXIT o un error
*/
int operate(thread_t * _thread, enum operation _op, int _val1, int _val2, int *_res, int * val){
    int ret = 0;
    int physical_dir;
    switch (_op)
    {
    case ADD:
        (*_res) = _thread->rn[_val1] + _thread->rn[_val2];
        printf("Sumando r%d y r%d para crear %d\n", _val1, _val2, (*_res));
        break;
    case ST:
    // Cambiar el valor en memoria
	// Acceder a la direccion fisica de memoria y cambiar el valor
        (*val) = get_physical(_thread, _val1, &physical_dir);
        printf("Accediendo a la posicion de memoria virtual %08X, physical %08X y colocando %d\n", _val1, physical_dir, _thread->rn[_val2]);
        set_at_dir(physical_dir,_thread->rn[_val2]);
        break;
    case LD:
        (*val) = get_physical(_thread, _val1, &physical_dir);
        printf("Accediendo a la posicion de memoria virtual %08X, physical %08X\n", _val1, physical_dir);
        // Coger el valor desde la memoria
        (*_res) = get_at_dir(physical_dir);
        printf("Dato obtenido %d\n", (*_res));
        break;
    case EXIT:
        // Se debe parar la ejecucion, el programa ha terminado
        printf("EXIT\n");
        ret = -1;
        break;
    default:
        printf("No se como he acabado aquí\n");
        ret = -1;
        break;
    }
    return ret;
}

/*
Guarda los resultados de las operaciones en sus registros correspondientes
*/
void write_results(thread_t *_thread, int *_res, int * _rd){
    // guardar resultado en el registro
    // por ejemplo, el resultado de la suma o el load
    // el store tiene que cambiar la memoria pero en OP
    // acceder al registro del indice rd y meter _res
    if ((*_rd) < 0 || (*_rd) > 15)
    {
	printf("Indice fuera de rango %d\n", (*_rd));
	return;
    }
    _thread->rn[(*_rd)] = (*_res);
}
