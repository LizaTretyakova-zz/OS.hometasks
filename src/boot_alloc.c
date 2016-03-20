#include <stdint.h>
#include "boot_alloc.h"
#include "mmap.h"
#include "setup_serial.h"

uint64_t boot_allocate(uint64_t requested) {
    unsigned int i = 0;
    for (; i < mmap_size; ++i) {
        if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE && requested <= mmap[i].len) {
            uint64_t result = mmap[i].addr;

            mmap[i].addr += requested;
            mmap[i].len -= requested;

            println("Om-nom'ed a part of memory by boot allocator:");
            print_mmap();

            return result;
        }
    }
    println("Not enough memory -- stopping the system *O*");
    halt();
    return 0;
}


