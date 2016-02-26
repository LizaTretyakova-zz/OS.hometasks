#include "setup_idt.h"
#include "interrupt.h"
#include "memory.h"

#define DPL 0
#define SPF (1 << 15)
#define IST 0
#define TYPE (14 << 8) // 0b1110

#define FIRST_PART 19
#define SECOND_PART 32

#define TABLE_SIZE 256

typedef void (*int_handler) (void);

void handler(void);
void pit_handler(void);

void init_entry(struct IDTEntry* single_entry, uint64_t offset) {
	//uint64_t offset = (uint64_t) handler;

	single_entry->reserved = 0;
	single_entry->offset0 = (offset >> 32);
	single_entry->offset1 = ((offset << 32) >> 48);
	single_entry->flags = SPF | DPL | TYPE | IST;
	single_entry->segment_selector = KERNEL_CODE;
	single_entry->offset2 = offset & 0xffff;
}

void setup_idt() {
	static struct IDTEntry idt[TABLE_SIZE];
	static struct idt_ptr idtd;

	for(int i = 0; i < FIRST_PART; ++i) {
		init_entry(idt + i, (uint64_t)handler);
	}
	init_entry(idt + SECOND_PART, (uint64_t)pit_handler);
	for(int i = SECOND_PART + 1; i < TABLE_SIZE; ++i) {
		init_entry(idt + i, (uint64_t)handler);
	}

	idtd.base = (uint64_t)idt;
	idtd.size = sizeof(idt) - 1;
	set_idt(&idtd);
}

