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
	int i = 0;
	while(string[i] != 0) {
		if((in8(0x3f + 5)) & (1 << 5)) {
			out8(0x3f8 + 0, string[i]);
			++i;
		}
	}
	while(!((in8(0x3f8 + 5)) & (1 << 5))) {
		continue;
	}
	out8(0x3f8 + 0, '\n');
}
