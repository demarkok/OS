#include "com.h"
#include "interrupts.h"
#include "pit.h"
#include "printf.h"
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

void testPrintf() {
	printf("\n\n---TEST---\n");
	int a = -5;
	long long b = 123456789123456789;
	unsigned long long c = -1ll;
	char s[4] = "lol";
	printf("a = %d\nb = %lld\nc = %llx\ns = %s\ns[1]=%c\n", a, b, c, s, s[1]);
	
	char buf[10];
	int read = snprintf(buf, 7, "a = %d\nb = %lld\nc = %llx\ns = %s\ns[1]=%c\n", a, b, c, s, s[1]);
	buf[read] = 0;
	printf("buf = %s\n", buf);

	printf("---TEST---\n\n");
}


void main(void)
{
	qemu_gdb_hang();

	initCOM();

	printf("START\n");

	// testPrintf();

	initIdt();
	initPic();
	initPit();
	
	__asm__ volatile("int $100");
	

	printf("done\n");
	
	while (1);
}
