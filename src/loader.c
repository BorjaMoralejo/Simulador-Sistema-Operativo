#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "physical.h"
#include "queue.h"
#include "estructuras.h"
#include "globals.h"
#include "randomp.h"

char line[8];
extern unsigned char *memoria;


int load_program(int _n, pid_t2 _pid);

int loader_flag = 0;
pthread_mutex_t loader_mtx;
pthread_cond_t loader_cond;

void init_loader(){
    pthread_mutex_init(&loader_mtx, NULL);
    pthread_cond_init(&loader_cond, NULL);
}

void * start_loader(void * _args){    
    pid_t2 pid_count = 0;
    pcb_t * pcb;
    int i = 0, ret;    
    int max_program = 999;
    while (1)
    {
        pthread_mutex_lock(&loader_mtx);
        loader_flag = 1;
        // esperando al timer
        while(loader_flag == 1)
            pthread_cond_wait(&loader_cond, &loader_mtx);


        if(i != max_program)
        {
            printf("Cargando programa %d...\n", i);

            ret = load_program(i, pid_count);
            switch (ret)
            {
            case 0: // Programa cargado correctamente
                printf("Programa cargado correctamente %3d!\n", i);
                i++;
                pid_count++;
                break;
            case -1: // No hay espacio en la memoria, esperar
                printf("No hay espacio en la memoria! %3d\n", i);
                break;
            case -2: // No hay hueco en la cola, esperar
                printf("No hay hueco en la cola maestra: %3d\n", i);
                break;
            case -3: // No hay PCBs libres, esperar
                break;
            case -4: // No existe ese programa!
                printf("No existe programa %3d!\n", i);
                break;
            default:
                printf("Default en el switch de retorno de load_program\n");
                break;
            }
            
        }else {
            // FIN del programa de LOADER
            printf("No hay más programas\n");
        }
        pthread_mutex_unlock(&loader_mtx);
    }
}

char prog_name[128];

/* 
Carga el programa con el número _n en memoria si hay hueco.
Devuelve: 
0 al cargarlo correctamente. 
-1 por falta de espacio en la memoria.
-2 por falta de huecos en la cola del scheduler maestro.
-3 no hay más PCBs libres.
-4 No existe el programa
*/
int load_program(int _n, pid_t2 _pid){
      

    unsigned int code_start, code_size, data_start, data_size;
    unsigned int dir_code, dir_data;
    unsigned int binary;
    FILE* fd;
    sprintf(prog_name, "programs/prog%03d.elf", _n);

    /*(hilo);

    Como carga el programa en memoria:
    Recibiendo hilo destino y numero de programa válido comprueba los siguientes campos antes de continuar.
    Primero comprueba que haya hueco en la cola del scheduler maestro.
    Segundo comprueba que la pool de PCBs no este vacia, es decir, que consiga un elemento PCB donde cargar el programa.
    Después abre el fichero con el número de programa en su nombre y se pone a obtener los datos como inicio y tamaño de parte de código.
    Repite lo mismo con la parte de datos.
    Intenta reservar espacio para la parte de código y datos en la memoria física. 
    Si no encuentra datos y ya había resevado la de código, libera esa parte y manda código de error.
    Si todo continua sin problemas, utilizando los datos de tamaño calcula el número de páginas que se les va a asignar.
    Asigna los datos al mm del PCB, el inicio de direccionamiento virtual del código, datos y final de datos.
    En esa parte se crea la tabla de páginas del proceso y se asignan las direcciones físicas en las posiciones correspondientes a las de código y virtuales.

    */

    int total_page_number, code_pages, data_pages;
    pcb_t *pcb;
    unsigned int i;
    char found = 0;

    printf("Comprobando elementos de cola de scheduler \n");
    // Comprueba que haya hueco en cola del scheduler maestro
    if(q_pcb.nElem >= q_pcb.maxElem) // no hay hueco en la cola devolviendo -2
        return -2;

    printf("Comprobando PCBs \n");
    // Reserva PCB
    pcb = getPCB();
    if(pcb == NULL) // ho hay PCBs en reserva, devolviendo código de error -3
        return -3;
    

    // Primero debe cargar el programa en el simulador y sacar datos como:
    // Tamaño total de cada parte
    // .text comienzo y longitud
    // .data comienzo y longitud
    printf("Fopen %s\n", prog_name);
    // Abrir fichero
    if((fd = fopen(prog_name, "r")) == NULL) // Si no existe el fichero, devuelve -4
    {
        putPCB(pcb);// Liberando pcb
        return -4;
    }

    fscanf(fd, "%s %X %X", line, &code_start, &code_size);
    fscanf(fd, "%s %X %X", line, &data_start, &data_size);
    printf("code_start %d, code_size %d, data_start %d, data_size %d \n", code_start, code_size, data_start, data_size);
    printf("Comprobando hueco memoria \n");
    // Reservar hueco (si es que hay)
    if( check_space(code_size, &dir_code) != 0)
    {
        fclose(fd);// No hay hueco, salir, cerrar fichero -1
        putPCB(pcb);// Liberando pcb
        return -1;
    }
    printf("Comprobando hueco memoria 2 \n");
    // Volver a reservar hueco para datos, si no hay, liberar anterior
    if( check_space(data_size, &dir_data) != 0)
    {
        release_space(dir_code, code_size); // Liberando asignación de code en memoria
        putPCB(pcb);// Liberando pcb
        fclose(fd);// Cerrar fichero
        return -1;
    }

    // Mirar cuantas paginas hay
        // Se tiene que crear un malloc para alvergar el numero de paginas que hay
        // donde va por orden sin tener en cuenta el startdir
        // y lo que se busca es la PAGINA X esta en el marco Y

    // Calculando número total de páginas que se utilizan
    //Si el módulo es 0 quiere decir que es multiplo 256 y que no necesita sumar uno para el redondeo hacia arriba
    code_pages = (code_size / PAGE_SIZE) + ((code_size % PAGE_SIZE) == 0 ? 0 : 1);
    data_pages = (data_size / PAGE_SIZE) + ((data_size % PAGE_SIZE) == 0 ? 0 : 1);
    total_page_number =  code_pages + data_pages; 
    
    
    // Asignarlo a la estructura mm del pcb
    pcb->mm.code_p = code_start;
    pcb->mm.data_p = data_start;
    pcb->mm.end_p = data_start + data_pages * PAGE_SIZE;
    pcb->mm.pgb = malloc(total_page_number*sizeof(unsigned int));
    

    // Rellenando tabla de paginas
    // Parte de code
    for(i = 0; i < code_pages; i++)
        pcb->mm.pgb[i] = dir_code + i*PAGE_SIZE;

    // Parte de data
    for(i = 0; i < data_pages; i++)
        pcb->mm.pgb[(code_pages)+i] = dir_data + i*PAGE_SIZE;
    

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

    pcb->pid = _pid;
    // Asignando prioridad aleatoria y afinidad aleatoria en el caso de estar activados
    if (paramStruct.random_priority == -1)
        pcb->priority = 0;
    else pcb->priority = getRandom(0, paramStruct.random_priority);

    if (paramStruct.random_affinity)
        pcb->affinity = getRandom(0, paramStruct.n_core)+getRandom(0, paramStruct.n_cpu)*paramStruct.n_core;
    else 
        pcb->affinity = -1;
    
    // Inicializando status para cuando se cargue en el procesador
    pcb->state=PCB_STATE_IDLE;
    pcb->status.pc = code_start;
    pcb->status.ri = 0;
    for(i = 0; i < 15; i++)
        pcb->status.rn[i] = 0;

    // Se lo pasa al scheduler maestro
    enqueue(&q_pcb, pcb);
              


    // Cerrar fichero
    fclose(fd);
    return 0;
}

void unload_program(pcb_t * _pcb){

    unsigned int dir_code, dir_data, code_size, data_size;
    unsigned int code_pages;
    // Conseguir los datos de dir_code, dir_data, code_size y data_size 
    // para poder liberar los espacios en la memoria

    /* 
    Nota informativa sobre otras variables en mm_t:

    Para calcular el tamaño se han guardado el número de páginas en una variable
    La razón por la que se ha optado por guardar el número de páginas de cada apartado, de code y de data, es debido a la
    imposibilidad de saber cuanto ocupan en memoria con tan solo las direcciones de comienzo.
    Sea code_start, la dirección de comienzo de code, el valor 0x000100 y su tamaño 80 bytes, o sea, code_size 80
    Y sea data_start 0x000200 y data_size 400.
    A partir de esos datos podemos obtener que code_size es como mucho 256, nos sirve porque lo que nos interesa es el número
    de páginas que ocupa. 
    Pero, ¿Cómo podemos obtener que data_size es 400? Solo conocemos el valor de inicio y la tabla no controla cuantos elementos contiene
    Por lo tanto he añadido un campo más, end. Este valor en el ejemplo anterior tendría el valor de 0x000400.
    */

    code_size = _pcb->mm.data_p - _pcb->mm.code_p;
    data_size = _pcb->mm.end_p - _pcb->mm.data_p;
    printf("U1\n");
    code_pages = (code_size / PAGE_SIZE) + ((code_size % PAGE_SIZE) == 0 ? 0 : 1);

    // Dir_code es la dirección física de la parte de code y esta siempre será la primera dirección.
    dir_code = _pcb->mm.pgb[0];
    printf("U2\n");
    dir_data = _pcb->mm.pgb[code_pages];

    // Liberando memoria ocupada por el programa de la memoria fisica
    printf("U3 %d %d\n", dir_code, code_size);
    release_space(dir_code, code_size);
    printf("U4 %d %d\n", dir_data, data_size);
    release_space(dir_data, data_size);
    printf("U5\n");


    // Sacar de la tabla de páginas
    free(_pcb->mm.pgb);
    printf("U6\n");
    _pcb->state = PCB_STATE_DEAD;
    putPCB(_pcb);
    printf("U7\n");
}