#ifndef __BUDDY_H__
#define __BUDDY_H__

#include <stdint.h>

#define PAGE_SIZE 1024

typedef struct BuddyDescriptor {
	uint8_t rank;
	uint8_t free;
	struct BuddyDescriptor *prev, *next;
} __attribute__((packed)) BuddyDescriptor;


void initBuddy();
void* buddyAlloc(int rank);
void buddyFree(void *addr);

#endif /*__BUDDY_H__*/