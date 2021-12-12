#ifndef _PHYSICAL_H
#define _PHYSICAL_H 1

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

#endif