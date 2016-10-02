#include "com.h"
#include "interrupts.h"
#include "pit.h"
#include <assert.h>


// extern void (*overHandler) (void);



static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>


void main(void)
{
	qemu_gdb_hang();



	initCOM();
	writeStr("START\n\n");

	initIdt();
	initPic();
	initPit();
	__asm__ volatile("sti"); // unmask on the cpu
	__asm__ volatile("int $100");
	


	writeStr("done\n");

	while (1);
}
