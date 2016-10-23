#include "com.h"
#include "interrupts.h"
#include "pit.h"
#include "printf.h"
#include "memmap.h"
#include "buddy.h"
#include "commonTools.h"


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

void testBuddy() {

	for (int t = 0; t < 100; t++) {
		int n = 6;
		int *arr[1 << 6];

		for (int q = 0; q < (1 << n); q++) {
			arr[q] = buddyAlloc(2 + n);	
			for (int i = 0; i < (1 << n); i++) {
				arr[q][i] = n * q + i;
			}
		}

		for (int q = 0; q < (1 << n); q++) {
			for (int i = 0; i < (1 << n); i++) {
				assert(arr[q][i] == n * q + i);
			}
		}
		for (int q = 0; q < (1 << n); q++) {
			buddyFree(arr[q]);
		}
	}
	printf("[OK] buddy tested\n");
}


void main(void)
{
	qemu_gdb_hang();

	initCOM();

	printf("START\n");

	initMemoryMap();
	printMemoryMap();

	initBuddy();
	testBuddy();


	initIdt();
	initPic();
	initPit();
	
	__asm__ volatile("int $100");
	

	printf("done\n");
	
	while (1);
}
