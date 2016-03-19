#include <stdint.h>
#include <stddef.h>
#include "setup_serial.h"
#include "mmap.h"

#define FIRST_MB (1024 * 1024)

extern const uint32_t mboot_info;
extern char text_phys_begin [];
extern char bss_phys_end [];

mboot_mmap_entry mboot_mmap[MAX_SIZE];
mmap_entry mmap[MAX_SIZE];
uint32_t mmap_size;

void print_mmap_entry(mmap_entry* m);
void print_mboot_mmap_entry(mboot_mmap_entry* m);

int mmaps_present(uint32_t flags) {
   return ((flags >> 6) & 1);  
}

uint64_t max_of(uint64_t a, uint64_t b) {
    return (a > b) ? a : b;
}

void add_kernel() {

    uint64_t kernel_start = (uint64_t) text_phys_begin;
    uint64_t kernel_end = (uint64_t) bss_phys_end;

    mmap[mmap_size].addr = kernel_start;
    mmap[mmap_size].len = kernel_end - kernel_start;
    mmap[mmap_size].type = MULTIBOOT_MEMORY_KERNEL;
    ++mmap_size;
}

void process_mmap(mboot_mmap_entry* entry, uint16_t* in_kernel) {

    uint64_t kernel_start = (uint64_t) text_phys_begin;
    uint64_t kernel_end = (uint64_t) bss_phys_end;
    uint64_t end_addr = entry->addr + entry->len - 1;

    if(!(*in_kernel)) {
        if(end_addr < kernel_start || entry->addr >= kernel_end) {
            if(mmap_size == 0 && entry->addr >= kernel_end) {
                add_kernel();
            }

            mmap[mmap_size].addr = entry->addr;
            mmap[mmap_size].len = entry->len;
            mmap[mmap_size].type = entry->type;
            ++mmap_size;
            return;
        }

        mmap[mmap_size].addr = entry->addr;
        mmap[mmap_size].len = kernel_start - entry->addr;
        mmap[mmap_size].type = entry->type;
        ++mmap_size;

        add_kernel();

        *in_kernel = 1;

        if(end_addr < kernel_end) {
            return;
        }

        mmap[mmap_size].addr = kernel_end;
        mmap[mmap_size].len = end_addr - kernel_end;
        mmap[mmap_size].type = entry->type;
        ++mmap_size;

        *in_kernel = 0;
        return;
    }

    if(end_addr < kernel_end) {
        return;
    }

    mmap[mmap_size].addr = max_of(kernel_end, entry->addr);
    mmap[mmap_size].len = end_addr - mmap[mmap_size].addr;//kernel_end;
    mmap[mmap_size].type = entry->type;
    ++mmap_size;

    *in_kernel = 0;
    return;

}

void correct_mmap() {
    for(unsigned int i = 0; i < mmap_size; ++i) {
        if(mmap[i].addr >= FIRST_MB) {
            return;
        }
        if(mmap[i].addr + mmap[i].len > FIRST_MB) {
            mmap[i].len -= (FIRST_MB - mmap[i].addr);
            mmap[i].addr = FIRST_MB;
            return;
        }
        mmap[i].type = MULTIBOOT_MEMORY_RESERVED;
    }
}

void get_mmap() {
    multiboot_info_t* multiboot_info = (multiboot_info_t*) (intptr_t) mboot_info;
    mmap_size = 0;

    //to check that mmap_length and mmap_addr are correct
    //we need them to be present
    if(!mmaps_present(multiboot_info->flags)) {
	    println("No mmap present!");
        return;
    }
    println("\nInitial mmap:");

    uint64_t end = (uint64_t) multiboot_info->mmap_addr + (uint64_t) multiboot_info->mmap_length;
    uint16_t in_kernel = 0;
    uint64_t i = 0;
    for(mboot_mmap_entry* mmap_ptr = (mboot_mmap_entry*) (uint64_t) multiboot_info->mmap_addr;
        (uint64_t) mmap_ptr < end;
        mmap_ptr = (mboot_mmap_entry*) ((uint64_t) mmap_ptr + mmap_ptr->size + sizeof (mmap_ptr->size))) {
        mboot_mmap[i] = *mmap_ptr;

        //information and debug
        print_mboot_mmap_entry(mboot_mmap + i);

        //we think about kernel <3
        process_mmap(mboot_mmap + i, &in_kernel);
        ++i;
    }
    correct_mmap();
}

void print_mmap_entry(mmap_entry* m) {
    print("from ");
    print_llu_x(m->addr);
    print(" to ");
    print_llu_x(m->addr + m->len);
    switch (m->type) {
        case MULTIBOOT_MEMORY_KERNEL:
            println(": kernel");
            break;
        case MULTIBOOT_MEMORY_AVAILABLE:
            println(": available");
            break;
        case MULTIBOOT_MEMORY_RESERVED:
            println(": reserved");
            break;
    }
}

void print_mboot_mmap_entry(mboot_mmap_entry* m) {
    print("from ");
    print_llu_x(m->addr);
    print(" to ");
    print_llu_x(m->addr + m->len);
    switch (m->type) {
        case MULTIBOOT_MEMORY_KERNEL:
            println(": kernel");
            break;
        case MULTIBOOT_MEMORY_AVAILABLE:
            println(": available");
            break;
        case MULTIBOOT_MEMORY_RESERVED:
            println(": reserved");
            break;
    }
}

void print_mmap() {
    println("\nActual mmap:");
    for(unsigned int i = 0; i < mmap_size; ++i) {
        print_mmap_entry(mmap + i);
	}
}


void halt() {
    while(1);
}


void assert_true(int st, char* str) {
    if(!st) {
        println("assertation failed");
        println(str);
        halt();
    }
}
