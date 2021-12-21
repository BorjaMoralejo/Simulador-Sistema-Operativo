
#ifndef _TLB_H
#define _TLB_H 1

#define BIT_MASK_VALID 0x80000000
typedef struct mmu_struct mmu_t;
typedef struct entrada_tbl entrada_tlb_t;


typedef struct mmu_struct
{
	entrada_tlb_t *entradas;
	int indice_entrada;
	int max_entradas;
}mmu_t;

typedef struct entrada_tbl
{
	unsigned int pagina;
	unsigned int marco;
}entrada_tlb_t;

void clean_TLB(thread_t * _thread);
void add_to_TLB(thread_t * _thread, unsigned int _page, unsigned int _physical);

#endif