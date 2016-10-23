#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <stdint.h>

typedef struct Descriptor {
	uint8_t rank;
	uint8_t free;
	struct Descriptor *prev, *next;
} __attribute__((packed)) Descriptor;


void initBuddy();
void* buddyAlloc(int rank);
void buddyFree(void *addr);

#endif /*__BUDDY_H__*/