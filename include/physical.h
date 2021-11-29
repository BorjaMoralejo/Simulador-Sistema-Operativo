#ifndef _PHYSICAL_H
#define _PHYSICAL_H 1

typedef struct huecos_node huecos_node_t;

typedef struct huecos_node
{
    huecos_node_t *next, *prev;
    int size;
    int dir;
}huecos_node_t;
#endif