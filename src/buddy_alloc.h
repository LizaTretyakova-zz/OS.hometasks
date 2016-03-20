#ifndef _BUDDY_ALLOC_
#define _BUDDY_ALLOC_

#include <stdint.h>
#include "memory.h"

struct page_descriptor_t {
  #define PAGE_DESCRIPTOR_RESERVED  0
  #define PAGE_DESCRIPTOR_AVAILABLE 1
  #define PAGE_DESCRIPTOR_FICT      2
    uint16_t is_free;
    uint16_t level;
    uint64_t pd_id;
    struct page_descriptor_t * next;
    struct page_descriptor_t * prev;
};
typedef struct page_descriptor_t page_descr;

void init_buddy();
phys_t buddy_allocate(unsigned int );
void buddy_deallocate(uint64_t , unsigned int );
void print_page_descr(page_descr* );
void print_level(int );
void print_pages();
void print_less_pages(int );
void print_pd_map();
void print_available_pages();
uint64_t addr_to_pd(phys_t );

#endif
