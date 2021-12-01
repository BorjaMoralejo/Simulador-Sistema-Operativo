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
typedef struct pagina pagina_t;
pagina_t *pages;
huecos_node_t * huecos, *last;
queue_hueco_t * q_huecos_reserva;


// Crear "huecos" de memoria utilizando una lista

// Algo que mire que la "lista de huecos" sea consecutiva y haga ese hueco más grande


void init_physical(){
    // TODO: meterlo en parametrizacion o en lo que sea
    int segmentos_max = 32;


    // Reservar memoria: 24 direccionamiento + 2 posicionamiento tamaño de palabra
    memoria = malloc(1<<26);
    // Crear lista de huecos
    inicializar_queue_hueco(q_huecos_reserva, segmentos_max);
    // Crear primer elemento y reservar direcciones de kernel
    huecos_node_t *primer = dequeueh(q_huecos_reserva);
    primer->dir = 0x3FFFFF;
    primer->size = 0xFFFFFF-0x3FFFFF;
    primer->next = NULL;
    primer->prev = NULL;
    huecos = primer;
    
}

/*
Comprueba, siguiendo la política worst fit, si hay un hueco con el espacio requerido. 
En caso de encontrar el hueco devuelve la posición y en caso contrario devuelve -1.
*/
int check_space(unsigned int _req_space){
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
        ret = max->dir;
    }else if(max->size >redondeo_arriba*PAGE_SIZE)   // Decrementa tamaño y recalcula la dirección
    { 
        max->size -= redondeo_arriba*PAGE_SIZE;
        max->dir += redondeo_arriba*PAGE_SIZE;
        ret = max->dir;
    }
    return ret;
}

/*
Recupera el espacio que habia reservado para esa dirección y si es posible lo combina con otros nodos.
*/
void release_space(int _dir, int _size){
    huecos_node_t * p = huecos;
    int found = 0;

    // Comprobar que alguno sumando dir+size de esa posicion para apendizarlo
    while (found == 0 && p->next != NULL)
    {
        if (p->dir+p->size == _dir)
        {
            found = 1;
            p->size += _size;
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
      p->size = _size;
    }
}

