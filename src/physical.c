#include <stdlib.h>
#include "physical.h"
#include "queue.h"

//- [ ] Bus de direcciones 24 bits, tamaño palabra (4 bytes)
//	- [ ] Pagina de 256 bytes
//	- [ ] Espacio reservado de memoria 0x000000 0x3FFFFF
//		- [ ] Politica de meter (mirar tema 3)
//	- [ ] Cargar programas en memoria
//	- [ ] Descargar programas en memoria

#define PAGE_SIZE 256
#define WORD_SIZE 4

#define RESERVED_START 0x000000
#define RESERVED_END   0x3FFFFF

unsigned char *memoria;
unsigned char *ptbr_p;
typedef struct pagina pagina_t;
pagina_t *pages;
huecos_node_t * huecos, *last;
queue_hueco_t * q_huecos_reserva;


// Crear "huecos" de memoria utilizando una lista

// Algo que mire que la "lista de huecos" sea consecutiva y haga ese hueco más grande


void init_physical(){
    
    // TODO: meterlo en parametrizacion o en lo que sea
    int segmentos_max = 32;
    

    // Reservar memoria: 24 direccionamiento + tamaño de palabra
    memoria = malloc(1<<24);
    // Crear lista de huecos
    inicializar_queue_hueco(q_huecos_reserva, segmentos_max);
    // Crear primer elemento y reservar direcciones de kernel
    huecos_node_t *primer = dequeueh(q_huecos_reserva);
    primer->dir = 0x3FFFFF;
    primer->size = 0xFFFFFF-0x3FFFFF;
    primer->next = NULL;
    primer->prev = NULL;
    huecos = primer;

    // Inicializando PTBR

    /*
    Palabras de 4B
    Pagina de 256B
    Direccionamiento fisico y virtual 24 bits

    Espacio logico: 24 bits, 16MB
    Tamaño pagina offset: 8 bits
    Numero de paginas: 16 bits, 2 ^16
    Espacio fisico: 24 bits, 16MB los mismos
    Tamaño: 16 bits y 8 offset
    Numero de marcos: 16 bits
    Numero PTE: 2 ^16
    Tamaño PTE: 2B
    Tamaño PageTable: 2 ^16 * 2B = 2 ^17B    
    */

    // 0x000000 0x3FFFFF Reservado para kernel, son 22bits
    ptbr_p = &memoria[0]; 

    // Entrada de la tabla de paginas (lo que hay en la TLB)
    // V:       valided
    // !P:       presencia
    // !R:       referenciada
    // D:       dirty
    // !Prot:    bits de proteccion
}

/*
Comprueba, siguiendo la política worst fit, si hay un hueco con el espacio requerido. 
En caso de encontrar el hueco devuelve la posición en _dir y en caso contrario devuelve -1.
*/
int check_space(unsigned int _req_space, int *_dir){
    int ret = -1;
    int redondeo_arriba;

    huecos_node_t * p = huecos;
    huecos_node_t * max = huecos;

    // pues eso, comprueba el hueco
    // politica worst fit, encontrar hueco de tamaño máximo y comprobar si entra o no
    while(p->next != NULL)
    {
        if(p->size > max->size)
            max = p;
        p = p->next;
    }

    redondeo_arriba = _req_space / PAGE_SIZE;
    if (_req_space % PAGE_SIZE != 0)
        redondeo_arriba++;

    if(max->size == redondeo_arriba*PAGE_SIZE)       // Justo el espacio necesario, sacar nodo
    {
        enqueueh(q_huecos_reserva, max);
        (*_dir) = max->dir;
        ret = 0;
    }else if(max->size >redondeo_arriba*PAGE_SIZE)   // Decrementa tamaño y recalcula la dirección
    { 
        max->size -= redondeo_arriba*PAGE_SIZE;
        max->dir += redondeo_arriba*PAGE_SIZE;
        (*_dir) = max->dir;
        ret = 0;
    }
    return ret;
}

/*
Recupera el espacio que habia reservado para esa dirección y si es posible lo combina con otros nodos.
*/
void release_space(int _dir, int _size){
    huecos_node_t * p = huecos;
    int found = 0;
    int i;
    int tam;
    tam = _size / PAGE_SIZE;
    if (_size % PAGE_SIZE != 0)
        tam++;


    // Comprobar que alguno sumando dir+size de esa posicion para apendizarlo
    while (found == 0 && p->next != NULL)
    {
        if (p->dir + p->size == _dir)
        {
            found = 1;
            p->size += tam*PAGE_SIZE;
        }
        p = p->next;
    }

    // Si ninguno da este valor, se coge un nodo y se asigna _dir y size
    if (found == 0)
    {
      p = dequeueh (q_huecos_reserva);
      last->next = p;
      p->next = NULL;
      p->prev = last;
      last = p;
      p->dir = _dir;
      p->size = tam*PAGE_SIZE;
    }

    // Quita la entrada de la tabla de paginas
    for(i = 0; i < 1 << 16; i++)
    {
        if(ptbr_p[i] == _dir)
            ptbr_p[i] = 0xFFFFFF;
    }
    
}


