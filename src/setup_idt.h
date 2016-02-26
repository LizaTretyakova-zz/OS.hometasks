#ifndef _SETUP_IDT_
#define _SETUP_IDT_

#include <stdint.h>

struct IDTEntry {
	uint16_t offset2;
	uint16_t segment_selector;
	uint16_t flags;
	uint16_t offset1;
	uint32_t offset0;
	uint32_t reserved;
} __attribute__((packed));

void setup_idt(void);
void send_interrupt(void);
#endif
