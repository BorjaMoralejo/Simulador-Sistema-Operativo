
#include "estructuras.h"
// Añadir bits de válido


/*
Añade la pagina y su traducción a física en la TLB siguiendo la política de reemplazo FIFO
*/
void add_to_TLB(thread_t * _thread, unsigned int _page, unsigned int _physical){
/*
    int i;
    int resultado;
    char found = 0;
    int LRU_busqueda;
    i = 0;
    // Buscar entrada inválida
    while (found == 0 && i < 64)
    {
        resultado = (_thread->mmu->entradas[i].pagina & 0xFF000000) >> 24;
        // Comprueba si está libre
        if((resultado & 0x80) == 0x00)
            found = 1;
        else 
            i++;
    }

    if(found == 0)
    {

        LRU_busqueda = 0x3F;
        // Buscar LRU y segunda oportunidad
        // Busca uno que sea 111111 y ese que se va si la segunda oportunidad es 0
        while (found == 0 && LRU_busqueda < 0)
        {
            i = 0;
            while (found == 0 && i < 64)
            {
                resultado = (_thread->mmu->entradas[i].pagina & 0xFF000000) >> 24;
                // Comprueba si tiene segunda oportunidad y comprueba si esta en el nivel de LRU de busqueda
                if((resultado & 0x40 == 0x00) && (resultado & LRU_busqueda) == LRU_busqueda)
                    found = 1;
                else 
                    i++;
                
            }
            if (found == 0)
                LRU_busqueda--;
        }   
    }
    if(found == 1)
    {
        // se añade en la posición i
    }else {
        // Algo ha ido muy mal
    }
*/
    
    // Añadiendo entrada y colocando bit de validez
    _thread->mmu->entradas[_thread->mmu->indice_entrada].pagina = BIT_MASK_VALID | _page;
    _thread->mmu->entradas[_thread->mmu->indice_entrada].marco = _physical;
    _thread->mmu->indice_entrada = (_thread->mmu->indice_entrada + 1) % _thread->mmu->max_entradas;    

}


/*
La TLB no tiene en cuenta de que proceso es la entrada en la TLB.
Una forma para evitar problemas es limpiando la TLB al hacer cambio de contexto.
*/
void clean_TLB(thread_t * _thread){
    // poner todos los bits de validez a 0
    int i;
    for (i = 0; i < _thread->mmu->max_entradas; i++)
        _thread->mmu->entradas[i].paginas = _thread->mmu->entradas[i].paginas & 0x7FFFFFFF;
}
