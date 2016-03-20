#include "ioport.h"
#include "setup_pic.h"
#include "setup_serial.h"
#include "setup_idt.h"
#include "setup_pit.h"
#include "mmap.h"
#include "buddy_alloc.h"


void easy_test_buddy(unsigned int );
void stronger_test_buddy(unsigned int );


void main(void)
{ 
	__asm__("cli");

	setup_serial();
	println("Initialized serial...");

	setup_idt();
	println("Initialized idt...");

	//__asm__("sti");
	//send_interrupt();

	setup_pic();
	println("Initialized pic...");
	
	setup_pit();
	println("Initialized pit...");

	__asm__("sti");
	
    init_buddy();
    //easy_test_buddy(3);
    //stronger_test_buddy(0);
    println("Initialized buddy allocator...");

	while (1); 
}


void easy_test_buddy(unsigned int level) {
    /** easy test */
    print("easy test at level ");
    print_llu(level);
    println("");
    print("before the test: ");
    print_pd_map();
    print_less_pages(level);
    phys_t addr = buddy_allocate(level);
    print("requested level ");
    print_llu(level);
    print(" and got addr ");
    print_llu_x(addr);
    print(", i.e. pd ");
    print_llu(addr_to_pd(addr));
    print("\n");
    print("during the test: ");
    print_pd_map();
    print_less_pages(level);
    buddy_deallocate(addr, level);
    print("after the test: ");
    print_pd_map();
    print_less_pages(level);
}


void stronger_test_buddy(unsigned int level) {
    /** stronger test */
    print("stronger test at level ");
    print_llu(level);
    println("");
    print("before the test: ");
    print_pd_map();
    print_less_pages(level);
    phys_t a[100];
    for(int i = 0; i < 100; ++i) {
        a[i] = buddy_allocate(level);
    }
    print("during the test: ");
    print_pd_map();
    print_less_pages(level);
    for(int i = 0; i < 100; ++i) {
        buddy_deallocate(a[i], level);
    }
    print("after the test: ");
    print_pd_map();
//    print_less_pages(level);
    print_pages();
}
