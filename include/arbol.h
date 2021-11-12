#ifndef _ARBOL_H
#define _ARBOL_H

#include "estructuras.h"

typedef struct tree_node tree_node_t;
typedef struct tree tree_t;


struct queue_nodos_arbol
    tree_node_t ** collection;

}

// los nodos y un pool o algo
typedef tree_node struct {
    tree_node_t *parentn, *leftn, *rightn;
    pcb_t pcb;
} tree_node_t;

typedef tree struct {
    queue_nodos_arbol queue;
    tree_node_t * root;
} tree_t;


// Inicializa el arbol y su piscina de elementos
void init_arbol(tree_t *_tree, int _maxElem);

// funcion de introducir nodo
void addNode(tree_node_t *_root, pcb_t *_elem);

// funcion de sacar nodo
pcb_t * removeNode(tree_node_t *_toRemove, tree_t _tree);


#endif