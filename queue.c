/* queue.c 
 * Asignatura: SO
 * Autor: Borja Moralejo Tobajas
 * 
 * Fichero .c donde están las utilidades de la estructura de datos: cola FIFO
 */


// out 		- es la primera posición de la cola
// in  		- es la última posición de la cola
// start 	- es el puntero de la memoria
// n_elementos 	- número de elementos actuales
// size 	- tamaño máximo
typedef struct cola{
	void *out;
	void *in;
	void *start;
	int n_elementos;
	int size;
	void *mallocOfType;
}queue_t;


void enqueue(queue_t _qu*, void *_elemento){
	_qu->in = _elemento;

}

void dequeue(queue_t _qu*, void **_elemento){
	_elemento* = qu->top;
	printf("%d\n", sizeof(_ret));
	top-=sizeof(_ret);
	
	//return _ret;
}
