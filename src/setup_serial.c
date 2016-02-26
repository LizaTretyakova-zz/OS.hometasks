#include "ioport.h"
#include "setup_serial.h"

void setup_serial() {
	out8(0x3f8 + 3, 0x83);//0b10000011); // Enabling division coefficient in 0 and 1 ports
	out8(0x3f8 + 0, 0x18); // Just a valid value for the coefficient
	out8(0x3f8 + 1, 0x00);	
	
	out8(0x3f8 + 3, 3);
	out8(0x3f8 + 1, 0x00);//0b00000000); // Interrupt Enable Register
}

void println(char* string) {
	for(int i = 0; string[i] != 0; ++i) {
		out8(0x3f8 + 0, string[i]);
	}
	out8(0x3f8 + 0, '\n');
}
