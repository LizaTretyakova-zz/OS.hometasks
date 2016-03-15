#include <stdint.h>
#include <stddef.h>
#include "setup_serial.h"
#include "mmap.h"

#define BASE_ADDR_OFFSET_32 (0 + 4)
#define LENGTH_OFFSET_32 (2 + 4)
#define TYPE_OFFSET_32 (4 + 4)

#define KERNEL 0
#define AVAILABLE 1

extern const uint32_t mboot_info;
extern struct mmap_entry* mmap;
extern uint64_t mmap_length;
extern char text_phys_begin [];
extern char bss_phys_end [];

struct 

#define MAP_START_PTR  ((uint32_t*) (uint64_t)mboot_info)
#define MMAP_LENGTH_PTR ((uint32_t*) (uint64_t) (MAP_START_PTR + 44))
#define MMAP_ADDR_PTR (* (uint32_t*) (uint64_t) (MAP_START_PTR + 48))


int mmaps_present(uint32_t flags) {
   return ((flags >> 6) & 1);  
}

void get_mmap() {
    uint32_t flags = *MAP_START_PTR;
    
    //to check that mmap_length and mmap_addr are correct
    //we need them to be present
    if(!mmaps_present(flags)) {
	println("No mmap present!");
        return;
    }
    
    mmap_length = *MMAP_LENGTH_PTR;
    uint32_t mmap_ptr = MMAP_ADDR_PTR;
    struct mmap_entry mmap_array[mmap_length + 1];
    uint64_t kernel_start = * (uint64_t*) text_phys_begin;
    uint64_t kernel_end = * (uint64_t*) bss_phys_end
    
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
    
    //return mmap;
}

void print_mmap() {
    println("Now you'll see the map:");
	char str[16];
	for(int i = 0; i < mmap_length; i++) {
	    println(str, "%lld - %lld, ", mmap[i].base, mmap[i].length);
	    switch (mmap[i].type) {
	        case MULTIBOOT_MEMORY_KERNEL:
	            println("kernel");
	            break;
	        case MULTIBOOT_MEMORY_AVAILABLE:
	            println("available");
	            break;
	        case MULTIBOOT_MEMORY_RESERVED:
	            println("reserved");
	            break;
	    }
	}
}
