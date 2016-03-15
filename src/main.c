#include "ioport.h"
#include "setup_pic.h"
#include "setup_serial.h"
#include "setup_idt.h"
#include "setup_pit.h"
#include "mmap.h"

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
	
	get_mmap();
    print_mmap();

	while (1); 
}


