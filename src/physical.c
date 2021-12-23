#include <stdlib.h>
#include <stdio.h>
#include "physical.h"
#include "queue.h"

//- [ ] Bus de direcciones 24 bits, tamaño palabra (4 bytes)
//	- [ ] Pagina de 256 bytes
//	- [ ] Espacio reservado de memoria 0x000000 0x3FFFFF
//		- [ ] Politica de meter (mirar tema 3)
//	- [ ] Cargar programas en memoria
//	- [ ] Descargar programas en memoria



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
    

    // Reservar memoria: 24 direccionamiento + tamaño de palabra
    memoria = malloc(1<<24);

    printf("M1\n");
    // Crear lista de huecos
    q_huecos_reserva = malloc(sizeof(queue_hueco_t));
    printf("M2\n");
    inicializar_queue_hueco(q_huecos_reserva, segmentos_max);
    printf("M3\n");
    // Crear primer elemento y reservar direcciones de kernel
    huecos_node_t *primer = dequeueh(q_huecos_reserva);
    printf("M4\n");
    primer->dir = 0x3FFFFF;
    primer->size = 0xFFFFFF-0x3FFFFF;
    primer->next = NULL;
    primer->prev = NULL;
    huecos = primer;
    last = huecos;
    printf("M5\n");
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
        (*_dir) = max->dir;
        max->dir += redondeo_arriba*PAGE_SIZE;
        ret = 0;
    }
    return ret;
}

/*
Recupera el espacio que habia reservado para esa dirección y si es posible lo combina con otros nodos.
*/
void release_space(int _dir, int _size){
    huecos_node_t *p = huecos;
    huecos_node_t *nuestro = NULL, *to_remove = NULL;
    int found = 0;
    int i;
    int tam;
    tam = _size / PAGE_SIZE;
    if (_size % PAGE_SIZE != 0)
        tam++;

    printf("R1\n");
    found = 0;
    /* 
    Comprobar que alguno sumando dir+size de esa posicion para apendizarlo
    Existen dos posibilidades:
        - Se une con la de "arriba"
        - Se une con la de "abajo"
    ¿A qué me refiero con arriba y abajo?
    Imaginemos la memoria como una lista vertical.
    La dirección de "arriba" + size (del otro), que se extiende hacia abajo en la lista, 
    llega hasta justo _dir, este es el caso 1.
    Para el caso 2, llegamos desde la dirección _dir + _size hasta dirección de "abajo".
    No obstante, ¿Qué ocurre si está como un bocadillo? Las direcciones de "arriba" y "abajo"
    combinados con _dir y _size crean un solo bloque. En este caso, está la parte de remerging.

    
    */
    while (found == 0)
    {
        printf("%d + %d == %d?\n", p->dir, p->size, _dir);
        printf("%d + %d == %d?\n", _dir, _size, p->dir);
        if (p->dir + p->size == _dir)
        {
            printf("MERGING1\n");
            // Simplemente se le aumenta el size
            found = 1;
            p->size += tam*PAGE_SIZE;
            
            _dir = p->dir;
            _size = p->size;
            nuestro = p;
        }else if(_dir + _size == p->dir)
        {
            printf("MERGING2\n");
            // Se le aumenta el size y se le cambia la dirección de inicio
            found = 1;
            p->size += tam*PAGE_SIZE;
            p->dir = _dir;

            _dir = p->dir;
            _size = p->size;
            nuestro = p;
        }
        if(found == 0)
        if(p->next != NULL)
            p = p->next;
        else 
            found = -1;
    }

    // Remerging
    if(found == 1)
    {
        printf("Remerging\n");
        char did_something = 1; 
        while (did_something == 1)
        {
            did_something = 0;
            p = huecos;
            found = 0;
            while (found == 0)
            {
                printf("%d + %d == %d?\n", p->dir, p->size, _dir);
                printf("%d + %d == %d?\n", _dir, _size, p->dir);
                if (p->dir + p->size == _dir)
                {
                    printf("REMERGING1\n");
                    found = 1;
                    p->size += nuestro->size;
                    did_something = 1;
                    // Aqui se tiene que quitar el hueco nuestro
                    to_remove = nuestro;
                    nuestro = p;
                    _dir = p->dir;
                    _size = p->size;
                }else if(_dir + _size == p->dir)
                {
                    printf("REMERGING2\n");
                    found = 1;
                    nuestro->size += p->size;
                    did_something = 1;
                    // Aqui se tiene que quitar p de la lista
                    to_remove = p;
                    _dir = nuestro->dir;
                    _size = nuestro->size;
                }
                if(found == 0)
                {
                    if(p->next != NULL)
                        p = p->next;
                    else 
                        found = -1;
                }else // Eliminando hueco que sea
                {
                    if(to_remove == last && to_remove == huecos)
                    {
                        printf("Algo ha ido horriblemente mal, to_remove == last == huecos\n");
                    }else if (to_remove == last)
                    {
                        to_remove->prev->next = NULL;
                        last = to_remove->prev;
                        to_remove->prev = NULL;
                    }else if(to_remove == huecos)
                    {
                        to_remove->next->prev = NULL;
                        huecos = to_remove->next;
                        to_remove->next = NULL;
                    }else 
                    {
                        to_remove->next->prev = to_remove->prev;
                        to_remove->prev->next = to_remove->next;
                        to_remove->next = NULL;
                        to_remove->prev = NULL;
                    }
                    
                }
            }
        }
        
    }

    printf("R2\n");
    // Si ninguno da este valor, se coge un nodo y se asigna _dir y size
    if (found == -1)
    {
      p = dequeueh (q_huecos_reserva);
      printf("R2.2\n");
      last->next = p;
      printf("R3\n");
      p->next = NULL;
      p->prev = last;
      last = p;
      p->dir = _dir;
      p->size = tam*PAGE_SIZE;
      printf("dir: %d tam: %d size: %d\n", _dir, _size, tam*PAGE_SIZE);
    }
    printf("R4\n");
}

/*
Esta función devuelve el valor que contiene la memoria en la posición _marco.
La memoria es un array de unsigned chars, son bytes sueltos, y la memoria está direccionada a bytes
pero la palabra son 4 bytes.
Entonces en esta función lo unico que se hace es aumentar el offset de la dirección y va contruyendo el entero
*/
int get_at_dir(unsigned int _marco){
    printf("get_at_dir marco %u\n", _marco);
    int ret = 0x00000000;
    ret = ret | (memoria[_marco + 0] << 0);
    ret = ret | (memoria[_marco + 1] << 8);
    ret = ret | (memoria[_marco + 2] << 16);
    ret = ret | (memoria[_marco + 3] << 24);
    return ret;
}

/*
Debido al direccionamiento al byte y el tamaño de la palabra son 4 bytes, esta función
se encarga de descomponer el _valor que se va a almacenar en los 4 bytes que componen el int.
Se utilizan mascaras para ello.
*/
void set_at_dir(unsigned int _marco, int _valor){
    memoria[_marco + 0] = (unsigned char) ((_valor & 0x000000FF) >> 0 );
    memoria[_marco + 1] = (unsigned char) ((_valor & 0x0000FF00) >> 8 );
    memoria[_marco + 2] = (unsigned char) ((_valor & 0x00FF0000) >> 16);
    memoria[_marco + 3] = (unsigned char) ((_valor & 0xFF000000) >> 24);
}

