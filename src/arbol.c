/*
#include <stdio.h>
#include <unistd.h>
#include "arbol.h"
#include "estructuras.h"

// Forward declaring
int addNodeRecursivo(tree_t *_tree, pcb_t *_elem, tree_node_t *_nodo);

// Orden de menor a mayor, izquierda menor y derecha mayor

// Inicializa el arbol y su piscina de elementos
void init_arbol(tree_t *_tree, int _maxElem){
    int i;
    // inicializa la queue/pool
    tree_t * nodos = malloc( sizeof(tree_node_t) * _maxElem);
    _tree->queue->collection = malloc( sizeof(tree_node_t *) * _maxElem);
    for ( i = 0; i < _maxElem; i++)
    {
        _tree->queue.collection[i] = &nodos[i];
    }
    _tree->queue.n = 0;
    _tree->queue.maxN = _maxElem;
}

// Funcion de introducir nodo, worker
int addNode(tree_t *_tree, pcb_t *_elem){
    // Pilla nodo, mete elemento y busca ordenadamente
    tree_node_t * nodo = _tree->root;

    // Caso raiz vacia
    if(nodo != NULL)
    {
        return addNodeRecursivo(_tree, _elem, nodo);
    }
    else 
    {
        _tree->root = _tree->queue.collection[_tree->queue.n++];
        _tree->root->pcb = _elem;
    }

   return 0;
}

// Metodo recursivo de addNode
int addNodeRecursivo(tree_t *_tree, pcb_t *_elem, tree_node_t *_nodo){
  
    if ( _nodo->pcb->nice <= _elem->nice ) 
    {
        // Este se convierte en la hoja
        if ( _nodo->leftn == NULL)
        {
            if(_tree->queue.n == maxN)
            {
                fprintf(stderr, "No hay más nodos\n");
                return -1;
            }
            _nodo->leftn = _tree->queue.collection[_tree->queue.n++];
            _nodo->leftn->pcb = _elem;
            return 0;
        }
        else 
        {
            return addNodeRecursivo ( _tree, _elem, _nodo->leftn); // Recursion
        }
    }
    else if ( _nodo->pcb->nice > _elem->nice ) 
    {
        // Este se convierte en la hoja
        if ( _nodo->rigthn == NULL)
        {
            if(_tree->queue.n == maxN)
            {
                fprintf(stderr, "No hay más nodos\n");
                return -1;
            }
            _nodo->rightn = _tree->queue.collection[_tree->queue.n++];
            _nodo->rightn->pcb = _elem;
            return 0;
        }
        else 
        {
            return addNodeRecursivo ( _tree, _elem, _nodo->rigthn); // Recursion
        }
    } 

    return -1;
}

// funcion de sacar nodo
pcb_t* removeNode(tree_node_t *_toRemove, tree_t _tree){
    tree_node_t *rightmost;
    tree_node_t *leftmost;
    pcb_t * ret;
    int last = 0;

    if(_toRemove->leftn != NULL) // rojo
    {
        rightmost = getRightMost(_toRemove->leftn); // azul

        // comprobando que no sea el mismo el que es el mas derecho?
        if(rightmost != _toRemove->leftn)
        {
            // Dando los que cuelgan del que se va a colocar en posicion a su padre
            if(rightmost->leftn != NULL)
            {
                rightmost->parentn->rightn = rightmost->leftn; 
                rightmost->leftn->parentn = rightmost->parentn;
            }
            else 
                rightmost->parent->rightn = NULL;

            rightmost->leftn = _toRemove->leftn; // 1
            _toRemove->leftn->parent = rightmost; // 2
        }
        
        rightmost->rightn = _toRemove->rightn; // 3
        _toRemove->rightn->parent = rightmost; // 4        
        if(_toRemove == _tree->root)
            _tree->root = rightmost;
    }
    else 
    if(_toRemove->rightn != NULL)
    {
        leftmost = getLeftMost(_toRemove->rightn);
         // comprobando que no sea el mismo el que es el mas derecho?
        if(leftmost != _toRemove->rightn)
        {
            // Dando los que cuelgan del que se va a colocar en posicion a su padre
            if(leftmost->rightn != NULL)
            {
                leftmost->parentn->leftn = leftmost->rightn; 
                leftmost->rightn->parentn = leftmost->parentn;
            }
            else 
                leftmost->parent->leftn = NULL;

            leftmost->rightn = _toRemove->rightn; // 1
            _toRemove->rightn->parent = leftmost; // 2
        }
        
        leftmost->leftn = _toRemove->leftn; // 3
        _toRemove->leftn->parent = leftmost; // 4        
        if(_toRemove == _tree->root)
            _tree->root = leftmost;

    } else {
        // Es una hoja
        if(_toRemove != _tree->root)
        {
            if(_toRemove->parent->leftn == _toRemove)
                _toRemove->parent->leftn = NULL;
            if(_toRemove->parent->rightn == _toRemove)
             _toRemove->parent->rightn = NULL;
        }
        else // es el root
            last = 1; 

    }

    // Terminando extraccion de nodo
    if ( last == 1) _tree->root = NULL;

    if(_tree->smallest_nice == _toRemove)
        _tree->smallest_nice = getLeftMost(_tree->root);
    
    _tree->queue.collection[--_tree->queue.n] = _toRemove;
    ret = _toRemove->pcb;
    _toRemove->pcb = NULL;
    _toRemove->leftn = NULL;
    _toRemove->right = NULL;
    _toRemove->parentn = NULL;
    
    return ret;
}


tree_node_t * getRightMost(tree_node_t *_node){
    if(_node->rightn != NULL)
        return getRightMost(_node->rightn);
    else return _node;
}

tree_node_t * getLeftMost(tree_node_t *_node){
    if(_node->leftn != NULL)
        return getLeftMost(_node->leftn);
    else return _node;
}

*/