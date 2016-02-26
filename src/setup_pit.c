#include "setup_pit.h"
#include "ioport.h"
#include "setup_serial.h"
#include "setup_pic.h"

#define CONTROL_PORT 0x43
#define DATA_PORT 0x40

#define CONTROL_WORD (0 | (3 << 4) | (1 << 2))
#define DIV_LOW_BYTE 0xFF
#define DIV_HIGH_BYTE 0x00

void handle_pit_interrupt() {
	println("Meow :)");
	eoi();
}

void setup_pit() {
	out8(CONTROL_PORT, CONTROL_WORD);
	
	out8(DATA_PORT, DIV_LOW_BYTE);
	out8(DATA_PORT, DIV_HIGH_BYTE);
}
