#include "physical.h"
#include "queue.h"
#include "estructuras.h"

	//- [ ] Cargar programas a simular en el simulador
	//	- [/] Pillar pcb
	//	- [ ] Tabla de paginas que se guarda en pgb
	//	- [/] Cargar ejecutable, copiar en memoria los segmentos de datos y codigo, acualizar tabla de paginas.
    //  - [ ] PID y esas cosas
char line[8];
extern unsigned char *memoria;

// Carga el programa con el número _n en memoria si hay hueco
void load_program(thread_t _thread, int _n){
      

    unsigned int code_start, code_size, data_start, data_size;
    unsigned int dir_code, dir_data;
    unsigned int binary;
    pcb_t *pcb;
    int i;
    char found = 0;

    // Reserva PCB
    pcb = getPCB();
    if(pcb == NULL)
    {
        // salir, cerrar fichero
    }


    // Primero debe cargar el programa en el simulador y sacar datos como:
    // Tamaño total de cada parte
    // .text comienzo y longitud
    // .data comienzo y longitud

    // Abrir fichero
    if((fd = fopen(prog_name, "r")) == NULL)
    {
        // Error, no existe ese programa, terminando?, cerrar fichero
    }

    fscanf(fd, "%s %X %X", line, &code_start, &code_size);
    fscanf(fd, "%s %X %X", line, &data_start, &data_size);
    
    // Reservar hueco (si es que hay)
    if( check_space(code_size, &dir_code) != 0)
    {
        // No hay hueco, salir, cerrar fichero
    }
    // Volver a reservar hueco para datos, si no hay, liberar anterior
    if( check_space(data_size, &dir_data) != 0)
    {
        release_space(dir_code, code_size);
    }

    // Mirar cuantas paginas hay

    // Direccion virtual 0 = pagina X?

    // Meter en la tabla de paginas
    while (found == 0 && i < 1 << 16)
    {
        if(_thread.PTBR[i] == 0xFFFFFF)
        {
            // Está libre, guardar direccion virtual y poner la fisica
            found = 1;
            _thread.PTBR[i]
        }else
            i++;
    }

    // Asignarlo a la estructura mm del pcb
    pcb->mm.code_p=code_start;
    pcb->mm.data_p=data_start;
    pcb->mm.pgb; // ?
    



    // Copiar datos de codigo directamente a memoria
    for(i = 0; i < code_size >> 2; i++){
        fscanf(fd, "%08X", &binary);      
        memoria[dir_code + i*4 + 3] = (binary >> 24 & 0xFF);
        memoria[dir_code + i*4 + 2] = (binary >> 16 & 0xFF);
        memoria[dir_code + i*4 + 1] = (binary >> 8  & 0xFF);
        memoria[dir_code + i*4 + 0] = (binary       & 0xFF);

    }

     // Copiar datos directamente a memoria
    for(i = 0; i < data_size >> 2; i++){
        fscanf(fd, "%08X", &binary);      
        memoria[dir_data + i*4 + 3] = (binary >> 24 & 0xFF);
        memoria[dir_data + i*4 + 2] = (binary >> 16 & 0xFF);
        memoria[dir_data + i*4 + 1] = (binary >> 8  & 0xFF);
        memoria[dir_data + i*4 + 0] = (binary       & 0xFF);

    }

    



    // Cerrar fichero
}

void unload_program(pcb_t * _pcb){

    // Sacar de la tabla de páginas
    // Marcar zonas como libre
    release_space(dir_code, code_size);

    // Juntar o no las listas con las existentes?
}