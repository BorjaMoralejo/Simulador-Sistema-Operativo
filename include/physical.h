#ifndef _PHYSICAL_H
#define _PHYSICAL_H 1

#define PAGE_SIZE 256
#define WORD_SIZE 4
typedef struct huecos_node huecos_node_t;

typedef struct huecos_node
{
    huecos_node_t *next, *prev;
    int size;
    int dir;
}huecos_node_t;

/*
Comprueba, siguiendo la política worst fit, si hay un hueco con el espacio requerido. 
En caso de encontrar el hueco devuelve la posición en _dir y en caso contrario devuelve -1.
*/
int check_space(unsigned int _req_space, int *_dir);

/*
Recupera el espacio que habia reservado para esa dirección y si es posible lo combina con otros nodos.
*/
void release_space(int _dir, int _size);

/*
Esta función devuelve el valor que contiene la memoria en la posición _marco.
La memoria es un array de unsigned chars, son bytes sueltos, y la memoria está direccionada a bytes
pero la palabra son 4 bytes.
Entonces en esta función lo unico que se hace es aumentar el offset de la dirección y va contruyendo el entero
*/
int get_at_dir(unsigned int _marco);


void set_at_dir(unsigned int _marco, int _valor);
#endif