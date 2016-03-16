#include <stdint.h>
#include <stddef.h>
#include "setup_serial.h"
#include "mmap.h"

extern const uint32_t mboot_info; // cast to structure?
extern char text_phys_begin [];
extern char bss_phys_end [];

mmap_entry mmap[MAX_SIZE];
uint32_t mmap_size;

int mmaps_present(uint32_t flags) {
   return ((flags >> 6) & 1);  
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
        
    uint64_t end = (uint64_t) multiboot_info->mmap_addr + (uint64_t) multiboot_info->mmap_length;
    for(mmap_entry* mmap_ptr = (mmap_entry*) (uint64_t) multiboot_info->mmap_addr;
        (uint64_t) mmap_ptr < end;
        mmap_ptr = (mmap_entry*) ((uint64_t) mmap_ptr + mmap_ptr->size + sizeof (mmap_ptr->size))) {
        mmap[mmap_size] = *mmap_ptr;
        ++mmap_size;
    }
/*    
    uint64_t kernel_start = * (uint64_t*) text_phys_begin;
    uint64_t kernel_end = * (uint64_t*) bss_phys_end;

    int i = 0;
    while(i <= mmap_length) {
        mmap[i].base = * (uint64_t*) (uint64_t)(mmap_ptr + BASE_ADDR_OFFSET_32);
        mmap[i].length = * (uint64_t*) (uint64_t) (mmap_ptr + LENGTH_OFFSET_32);
        mmap[i].type = * (uint32_t*) (uint64_t) (mmap_ptr + TYPE_OFFSET_32);
        mmap_ptr = (uint64_t)(mmap_ptr + * (uint64_t*) (uint64_t) mmap_ptr);
    
        if(mmap[i].base < kernel_start &&
           mmap[i].base + mmap[i].length < kernel_start) {
            ++i;
            mmap[i].base = kernal_start;
            mmap[i].length = kernel_start - kernel_end;
            mmap[i].type = KERNEL;
        }
        ++i;
    }
*/    
}

void print_mmap() {
    println("Now you'll see the map:");
    for(unsigned int i = 0; i < mmap_size; ++i) {
        print_llu(mmap[i].addr);
        print(" : ");
	    print_llu(mmap[i].len);
	    switch (mmap[i].type) {
	        case MULTIBOOT_MEMORY_KERNEL:
	            println(" kernel\n");
	            break;
	        case MULTIBOOT_MEMORY_AVAILABLE:
	            println(" available\n");
	            break;
	        case MULTIBOOT_MEMORY_RESERVED:
	            println(" reserved\n");
	            break;
	    }
	}
}
