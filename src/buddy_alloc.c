#include <stdint.h>
#include <stddef.h>
#include "boot_alloc.h"
#include "mmap.h"
#include "setup_serial.h"
#include "buddy_alloc.h"
#include "memory.h"


extern mmap_entry mmap[MAX_SIZE];
extern uint32_t mmap_size;

page_descr heads[MAX_SIZE];
page_descr* descr_table;
uint64_t dt_size;

void debug_print_level(unsigned int new_lvl, page_descr* , page_descr* );
void debug_print_pd_and_buddy(uint64_t , uint64_t );


// find the last available address,
// we will create descriptors for all this memory
// but then use only available ones
uint64_t find_max_available_address() {
    uint64_t result = 0;
    for(unsigned int i = 0; i < mmap_size; ++i) {
        if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
            result = mmap[i].addr + mmap[i].len;
        }
    }
    return result;
}


// pd is an index of descriptor in descr_table
uint64_t addr_to_pd(phys_t addr) {
    // use integer division to get a descriptor of the containing page
    return (addr / PAGE_SIZE);
}
// start addr
phys_t pd_to_addr(uint64_t pd) {
    return (pd * PAGE_SIZE);
}


// does not deal with availability of the blocks AT ALL!!!
void merge(uint64_t* pd, uint64_t* buddy, unsigned int new_lvl) {
    descr_table[*pd].level = new_lvl;
    descr_table[*buddy].level = new_lvl;

    //page_descr* pd_prev = descr_table[*pd].prev;
    //page_descr* buddy_next = descr_table[*buddy].next;
//    They are invalid!!! Anyway!!!

    // heads[i] will be a fictitious pointer, always existing
//    assert_true(pd_prev != NULL, "pd_prev != NULL");
    // next of pd_prev should be pd
//    assert_true(pd_prev->next == descr_table + *pd, "pd_prev->next == descr_table + *pd");
//    pd_prev->next = buddy_next;
    //although next may be abscent
//    if(buddy_next != NULL) {
//        buddy_next->prev = pd_prev;
//    }

    page_descr* new_prev = heads + new_lvl;
    assert_true(new_prev != NULL, "new_prev != NULL");
    page_descr* new_next = heads[new_lvl].next;
    // deal with prev
    descr_table[*pd].prev = new_prev;
    new_prev->next = descr_table + *pd;
    // deal with next
    descr_table[*pd].next = new_next;
    if(new_next != NULL) {
        new_next->prev = descr_table + *pd;
    }
}


uint64_t get_buddy(uint64_t pd, unsigned int lvl) {
    return pd ^ (1 << lvl);
}


void buddy_deallocate(uint64_t pd, unsigned int lvl) {
    descr_table[pd].is_free = PAGE_DESCRIPTOR_AVAILABLE;
    for(unsigned int i = lvl; i < MAX_SIZE - 1; ++i) {
        uint64_t buddy = get_buddy(pd, i);
        if(buddy >= dt_size ||
                !(descr_table[buddy].is_free == PAGE_DESCRIPTOR_AVAILABLE && descr_table[buddy].level == i)) {
            return;
        }
        merge(&pd, &buddy, i + 1);
    }
}


uint64_t activate_available_mem() {
    uint64_t result = 0;
    for(unsigned int i = 0; i < mmap_size; ++i) {
        if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
        // if we find a free part, we activate all contained pages
            for(unsigned int offset = 0; offset < mmap[i].len; offset += PAGE_SIZE) {
                buddy_deallocate(addr_to_pd(mmap[i].addr + offset), 0);
            }
        }
    }
    return result;
}


void set_page_descr(page_descr* src, unsigned int pd, uint16_t status, uint16_t lvl, page_descr* p, page_descr* n) {
    src->pd_id = pd;
    src->is_free = status;
    src->level = lvl;
    src->prev = p;
    src->next = n;
}


void init_buddy() {

    get_mmap();
    print_mmap();

    // request memory for page descriptors
    // requested pages for all memory until the last available address -- because I can
    dt_size = (find_max_available_address() / PAGE_SIZE) * sizeof(page_descr);
    descr_table = (page_descr*) boot_allocate(dt_size);

    // initialize dt
    page_descr* prev = heads + 0;
    for(unsigned int i = 0; i < dt_size; ++i) {
        set_page_descr(descr_table + i, i, PAGE_DESCRIPTOR_RESERVED, 0, prev, NULL);

        assert_true(prev != NULL, "prev != NULL");
        prev->next = descr_table + i;
        prev = descr_table + i;
    }

    //initialize heads
    set_page_descr(heads + 0, 0, PAGE_DESCRIPTOR_FICT, 0, NULL, descr_table + 0);
    for(int i = 1; i < MAX_SIZE; ++i) {
        set_page_descr(heads + i, 0, PAGE_DESCRIPTOR_FICT, i, NULL, NULL);
    }

    //initialize actually available descriptors
    activate_available_mem();

    print_pages();
}


// the user programmer should provide the correct level
// correct === the current level is not empty, while the level below is
void push_down(unsigned int level) {
    assert_true((heads[level]->next != NULL), "push_down first contract");
    assert_true((level > 0), "push_down second contract");
    assert_true((heads[level - 1]->next == NULL), "push_down third contract");

    uint64_t pd_id =  heads[level]->next->pd_id;
    uint64_t buddy_id = get_buddy(pd_id, level - 1);
    page_descr* pd = descr_table[pd_id];
    page_descr* buddy = descr_table[buddy_id];

    heads[level]->next = pd->next;
    if(pd->next != NULL) {
        pd->next->prev = heads[level];
    }

    // it is significant that previous level is empty
    heads[level - 1]->next = pd;
    set_page_descr(pd, pd->pd_id, PAGE_DESCRIPTOR_AVAILABLE, level - 1, heads[level - 1], buddy);
    set_page_descr(buddy, buddy->pd_id, PAGE_DESCRIPTOR_AVAILABLE, level - 1, pd, NULL);
}


// contract: level is not empty
phys_t extract_mem(unsigned int level) {
    assert_true((heads[level]->next != NULL), "extract_mem contract");

    page_descr* pd = heads[level]->next;
    heads[level]->next = pd->next;
    if(pd->next != NULL) {
        pd->next->prev = heads[level];
    }
    pd->is_free = PAGE_DESCRIPTOR_RESERVED;
    return pd_to_addr(pd->pd_id);
}


phys_t buddy_allocate(unsigned int lvl) {
    unsigned int k = lvl;
    while(k < MAX_SIZE && heads[k]->next == NULL) {
        ++k;
    }
    while(k > lvl) {
        push_down(k);
        --k;
    }
    return extract_mem(lvl);
}



void print_page_descr(page_descr* a) {
    print("(level: ");
    print_llu(a->level);
    print("; addr: ");
    print_llu_x(pd_to_addr(a->pd_id));
    print("; pd: ");
    print_llu(a->pd_id);
    print("; is_free: ");
    switch(a->is_free) {
        case PAGE_DESCRIPTOR_AVAILABLE:
            print("available");
            break;
        case PAGE_DESCRIPTOR_RESERVED:
            print("reserved");
            break;
        case PAGE_DESCRIPTOR_FICT:
            print("HEAD");
            break;
        default:
            print("something very bad happened: UNKNOWN_TYPE;\n stopping the system");
            halt();
    }
    println(") <--> ");
}


void print_level_var(int i, int print_reserved) {
    print("LEVEL ");
    print_llu(i);
    print(": ");
    page_descr* cur = heads + i;
    while(cur != NULL) {
        if(print_reserved || cur->is_free != PAGE_DESCRIPTOR_RESERVED) {
            print_page_descr(cur);
        }
        cur = cur->next;
    }
    println("\n");
}

void print_level(int i) {
    print_level_var(i, 1);
}

void print_pages() {
    for(int i = MAX_SIZE - 1; i > 0; --i) {
        print_level(i);
    }
}

void print_available_pages() {
    for(int i = MAX_SIZE - 1; i >= 0; --i) {
        print_level_var(i, 0);
    }
}


void debug_print_buddy_and_pd_lots(uint64_t pd, uint64_t buddy, unsigned int i) {
    print("i: ");
    print_llu(i);
    print(" pd: ");
    print_llu(pd);
    print(" buddy: ");
    print_llu(buddy);
    print(" dt_size: ");
    print_llu(dt_size);
    print(" is_free - buddy: ");
    print_llu(descr_table[buddy].is_free);
    print(" level - buddy: ");
    print_llu(descr_table[buddy].level);
    println("");
}
void debug_print_addr_and_pd(unsigned int i, unsigned int offset) {
    print("addr: ");
    print_llu_x(mmap[i].addr + offset);
    print(" pd: ");
    print_llu(addr_to_pd(mmap[i].addr + offset));
    println("");
}
void debug_print_level(unsigned int new_lvl, page_descr* pd_prev, page_descr* buddy_next) {
    if(new_lvl > 1) {
        println("***");
        print_level(new_lvl);
        print_level(new_lvl - 1);
        print("pd_prev: ");
        print_page_descr(pd_prev);
        print("buddy_next: ");
        print_page_descr(buddy_next);
    }
}
void debug_print_pd_and_buddy(uint64_t pd, uint64_t buddy) {
    print("pd: ");
    print_llu(pd);
    print("\nbuddy: ");
    print_llu(buddy);
    println("");
}

