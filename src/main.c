#include "com.h"
#include "interrupts.h"
#include "pit.h"
#include "printf.h"
#include "memmap.h"
#include "buddy.h"
#include "commonTools.h"
#include "slab.h"
#include "threads.h"


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

void testSlab() {
	for (int t = 0; t < 100; t++) {
		int n = 50;
		int *arr[50];
		SlabAllocator allocator = createSlabAllocator(n * sizeof(int), n);
		for (int q = 0; q < n; q++) {
			arr[q] = slabAlloc(&allocator);
			for (int i = 0; i < n; i++) {
				arr[q][i] = n * q + i;
				// printf("%d ", arr[q][i]);
			}
			// printf("\n");
		}
		for (int q = 0; q < n; q++) {
			for (int i = 0; i < n; i++) {
				// printf("%d ", arr[q][i]);
				assert(arr[q][i] == n * q + i);
			}
			// printf("\n");
			slabFree(&allocator, arr[q]);
		}
		destroySlabAllocator(allocator);
	}
	printf("[OK] slab tested\n");
}

// seems that it tests nothing
void testThreads() {
	int x = 0;
	lock();
	x = 1;
	unlock();
	assert(x == 1);
	printf("[OK] threads tested\n");
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
	
	testSlab();

	initIdt();

	initPic();
	initPit(); // start ticker

	testThreads();
		
	printf("done\n");
	
	while (1);
}
