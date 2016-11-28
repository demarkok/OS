#include "threads.h"

int claim = 0;

// threads manager works with interruptions, so we can just diable interruptions to lock 'mutex'  

void lock() {
	if (claim == 0) {
		__asm__ volatile("cli"); // disable interrupts
	}
	claim = 1;
}

void unlock() {
	claim = 0;
	__asm__ volatile("sti"); // enable interrupts
}
