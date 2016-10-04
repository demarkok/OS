#include "interrupts.h"
#include "desc.h"
#include "com.h"
#include "memory.h"
#include "ioport.h"
#include "commonTools.h"
#include "pit.h"
#include "printf.h"
#include <stdint.h>

// extern void (*overHandler) (void);

// extern uint64_t *overHandler;
extern uint64_t handlers[];

// extern void* handlers[N];

static desc_table_ptr idtPtr;

#define IDT_SIZE 256
#define INTERRUPT_GATE_TYPE 0b10001110
#define TRAP_GATE_TYPE 0b10001111



static idtEntry idt[IDT_SIZE];


void eoi(uint64_t id) {
	if (id >= masterFirstIntId && id < masterFirstIntId + 8) {
		out8(masterCommandPort, 0b00100000);
	}
	if (id >= slaveFirstIntId && id < slaveFirstIntId + 8) {
		out8(slaveCommandPort, 0b00100000);		
		out8(masterCommandPort, 0b00100000);
	}
}


void handler(uint64_t id) {

	if (id == masterFirstIntId) {
		tick();
	}
	else {
		printf("[INTERRUPT] #%d\n", id);
	}

	eoi(id);
}


void assert(int expr) {
	if (!expr) {
		printf("[ERROR] Assertion\n");
		while(1);
	}
	printf("[OK] Assertion\n");
}


void initIdt() {
	
	for (int i = 0; i < 256; i++) {
		uint64_t addr = handlers[i];
		idt[i].offset1 = submask(addr, 0, 15);
		idt[i].offset2 = submask(addr, 16, 31);
		idt[i].offset3 = submask(addr, 32, 63);
	
		idt[i]._trash1 = 0;
		idt[i]._trash2 = 0;	

		idt[i].segSelector = KERNEL_CS;
		idt[i].typeAndP = TRAP_GATE_TYPE;
		
	}

	idtPtr.size = sizeof(idt) - 1;
	idtPtr.addr = (uint64_t)idt;

	write_idtr(&idtPtr);

	writeStr("[OK] IDT initialized\n");
}

void initPic() {


	out8(masterCommandPort, 0b00010001);
	out8(masterDataPort, masterFirstIntId); // mapping master's legs into idt
	out8(masterDataPort, 0b00000100); // slave holds the 2-nd leg
	out8(masterDataPort, 1);

	out8(slaveCommandPort, 0b00010001);
	out8(slaveDataPort, slaveFirstIntId); // map slave's legs into idt
	out8(slaveDataPort, 2); // slave holds the 2-nd leg
	out8(slaveDataPort, 1);

	// mask each leg
	out8(masterDataPort, 0b11111111);
	out8(slaveDataPort, 0b11111111);

	__asm__ volatile("sti"); // unmask on the cpu
}
