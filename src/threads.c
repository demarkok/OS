#include "threads.h"


// threads manager works with interruptions, so we can just diable interruptions to lock 'mutex'  

void lock() {
	__asm__ volatile("cli"); // disable interrupts
}

void unlock() {
	__asm__ volatile("sti"); // enable interrupts
}
