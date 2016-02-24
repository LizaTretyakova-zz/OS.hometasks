#include "ioport.h"
#include "setup_pic.h"
#include "setup_serial.h"

void main(void)
{ 
	//setup_pic();
	setup_serial();
	out8(0x3f8 + 0, 'L');
	while (1); 
}


