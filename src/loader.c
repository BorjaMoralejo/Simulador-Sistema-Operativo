	//- [ ] Cargar programas a simular en el simulador
	//	- [ ] Pillar pcb
	//	- [ ] Tabla de paginas que se guarda en pgb
	//	- [ ] Cargar ejecutable, copiar en memoria los segmentos de datos y codigo, acualizar tabla de paginas.

// o loader


// Carga el programa con el número _n en memoria si hay hueco
void load_program(int _n){
      
    // Reservar hueco y asignarlo a la estructura mm
    // Meter en la tabla de paginas

    // Mirar si hay hueco (encontrar hueco segun politica)
    // creo que se pueden separar las partes en diferentes trozos
    // Abrir fichero
    // Copiar datos directamente a memoria
    // Cerrar fichero
}

void unload_program(){
    // Sacar de la tabla de páginas
    // Marcar zonas como libre
    // Juntar o no las listas con las existentes?
}