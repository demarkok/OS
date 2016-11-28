#include "threads.h"

int claim = 0;

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
