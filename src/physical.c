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


// Crear "huecos" de memoria utilizando una lista

// Algo que mire que la "lista de huecos" sea consecutiva y haga ese hueco más grande


void init_physical(){
    // Reservar memoria
    // Crear lista de huecos
}

void check_space(){
    // pues eso, comprueba el hueco
}
void load_program(){
    // Reservar hueco y asignarlo a la estructura mm
    // Meter en la tabla de paginas
}

void unload_program(){
    // Sacar de la tabla de páginas
    // Marcar zonas como libre
    // Juntar o no las listas con las existentes?
}
