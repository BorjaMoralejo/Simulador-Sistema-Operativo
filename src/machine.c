
#include "estructuras.h"
#include "tlb.h"
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

extern unsigned char *memoria;


enum operation{ADD, ST, LD, EXIT };

void init_thread(thread_t * _thread){
    int i;

    // inicializando vector de registros
    thread_l->rn = malloc(sizeof(int)*16);
    
    // asignando PTBR
    thread_l->PTBR = NULL;

    // generando mmu
    _thread->mmu = malloc(sizeof(mmu_t));

    _thread->mmu->max_entradas = 32;
    _thread->mmu->entradas = malloc(sizeof(entrada_tlb_t) * _thread->mmu->max_entradas);

    for ( i = 0; i < _thread->mmu->max_entradas; i++ )
    {
        _thread->mmu->entradas[i].pagina = 0xFFFF;
    }
}

int get_physical(thread_t *_thread, int _virtual){
    int i;
    char found = 0;
    unsigned dir_traducida = NULL;
    // pillar mmu

    // https://egela.ehu.eus/pluginfile.php/4977224/mod_resource/content/10/OS%20-%20Tema%203-1%20Sistema%20de%20gesti%C3%B3n%20de%20memoria.pdf
    // Pillar numero de pagina y offset

// esto esta mal, hay que rehacerlo, la PTBR no deberia de ser como esta hecha lmao

    // REHACER lo de tlb, hay que buscar usando el numero de pagina y traducirlo a numero de marco
    unsigned int pagina = _virtual & 0xFFFF00;
    unsigned int offset = _virtual & 0x0000FF;

    // Comprobar que la pagina sea del proceso?
    

    // mirar tlb en mmu y ver si existe la traduccion instantanea
    while (found == 0 && i < _thread->mmu.n_entradas)
    {
        // Si encuentra el numero de pagina en el tlb, se asigna el valor del TLB
        if((_thread->mmu->entradas[i].pagina & BIT_MASK_VALID) == BIT_MASK_VALID &&  (_thread->mmu.entradas[i].pagina & 0x00FFFFFF) == pagina)
        {
            dir_traducida = _thread->mmu.entradas[i].marco;
            found = 1;
        }
        i++;
    }
    

    

    // miss, deberia de bloquearse el programa (no tiene cache) (eso esta implementado, solo que esta random, esta en el clock)
    // De todos modos, busca la respuesta aqui para que al desbloquearse lo unico que tenga que hacer sera repetir esto
    if(found == 0)
    {
        // Al ser una simulación limitada, se asigna el valor de la tlb ahora 

        dir_traducida = _thread->PTBR[pagina];        
        // Si esta llena se va a ir sobreescribiendo 
        add_to_TLB(_thread, pagina, dir_traducida); 

    }

    dir_traducida |= offset;

    return dir_traducida;
}




/*
Hace el comando que le toca al thread.
Es posible que el comando sea EXIT y deba salir de la CPU.
*/ 
int do_command(thread_t * _thread){
    
    int * ri_dir; // ?????
    int ra, rb, rd, rs;
    enum operation op;
    // Hacer esto con funciones "virtuales" para tener un puntero que va a ir cambiado y de esta
    // forma poder ir avanzando de operacion por turno
    // Mejor no por las posibles dependencias que se pueden crear

    // B D L A E
    fetch(_thread);
    decode(_thread, &op);

    // Se tiene que obtener los indices de los registros
    load_op(_thread, op, &ra, &rb, &rd);

    if (operate (_thread, op, ra, rb, &rs) == -1)
    {
	    return -1;    // Exit
    }

    write_results(_thread, rs, rd);
    return 0;
}

/*
Coge la instruccion de memoria y la pone en RI
*/
void fetch(thread_t * _thread){
    int * instruccion;
    int virtual_dir;
    int physical_dir;
    // buscar la info en la posicion que sea
    int pos = _thread->pc++;

    // coger la direccion virtual del pc?
    (*_thread->enProceso)->mm.code_p[pos];

    // coger la instruccion y meterla en RI
	physical_dir = get_physical(_thread, virtual_dir);
    _thread->ri = get_at_dir[physical_dir]; // Acceder a physical_dir
}

/*
Decodifica el registro RI para saber que operación debe hacer
*/
void decode(thread_t * _thread, enum operation *_op){
    // decode, enviar a siguiente operacion que toque
    (*_thread->enProceso)->mm;
    enum operation op_enum;
    int op = _thread->ri & mask_decode_op;
    // Asignando la operacion
    switch(op){
        case ld:
            printf("LD\n");
            (*_op) = LD;
            break;
        case st:
            printf"ST\n");
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
	(*_ra) = _thread->ri & mask_decode_r2;
	(*_rb) = _thread->ri & mask_decode_r3;
	(*_rd) = _thread->ri & mask_decode_r1;
	break;
case ST:   // Debe cargar el indice de ra             revisarlo con el enunciado
	(*_ra) = _thread->ri & mask_decode_r2;
    (* rb) = _thread->ri & mask_decode_dir;
	break;
case LD:   // Debe cargar el indice de rd
	(*_rd) = _thread->ri & mask_decode_r1;
    (* rb) = _thread->ri & mask_decode_dir;
	break;
case EXIT: // No hace nada, no tiene registros destino ni origen
	
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
int operate(enum operation _op, int _val1, int _val2, int *_res, thread_t _thread){
    int ret = 0;
    int physical_dir;
    switch (_op)
    {
    case ADD:
        (*_res) = (*_val1) + (*_val2);
        break;
    case ST:
    // Cambiar el valor en memoria
	// Acceder a la direccion fisica de memoria y cambiar el valor
        physical_dir = get_physical(_thread, _val1);
        printf("Accediendo a la posicion de memoria virtual %d, physical %d y colocando \n", physical_dir, _val1, _val2);
        set_at_dir(physical_dir, _val2);
        break;
    case LD:
        physical_dir = get_physical(_thread, _val1);
        // Coger el valor desde la memoria
        (*_res) = get_at_dir(physical_dir);
        break;
    case EXIT:
        // Se debe parar la ejecucion, el programa ha terminado
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
