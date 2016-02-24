#include "ioport.h"
#include "setup_pic.h"

void setup_pic() {
	out8(0x20, 0x11);//0b00010001);// 1st command word, master
	out8(0xa0, 0x11);// 1st, slave
	
	out8(0x21, 0x20);// 2nd, master
	out8(0xa1, 0x28);// 2nd, slave

	out8(0x21, 0x4);// 3rd, master
	out8(0xa1, 0x2);// 3rd, slave

	out8(0x21, 0x1);// 4th, master
	out8(0xa1, 0x1);// 4th, slave
}

void eoi() {
	out8(0x20, 0x20);
	out8(0xa0, 0x20);
}
