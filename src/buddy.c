#include "buddy.h"
#include "memmap.h"
#include "printf.h"
#include "commonTools.h"
#include <stdint.h>

#define MAX_MAX_RANK 64
int maxRank = 0;
Descriptor *descriptors;
Descriptor *rankHead[MAX_MAX_RANK];


static void *firstPage;

#define PAGE_SIZE 1024

#define LOGICAL_START 0xffff800000000000ll
#define PHYS(x) ((uint64_t)x - LOGICAL_START)
#define LOGIC(x) ((uint64_t)x + LOGICAL_START)

#define INDEX(descrPtr) (descrPtr - descriptors)
#define BUDDY(descrPtr) (descriptors + ((1ll << ((descrPtr)->rank)) ^ INDEX(descrPtr)))
#define PAGE(descrPtr) (void*)((uint64_t)firstPage + (INDEX(descrPtr) * PAGE_SIZE))
#define DESCR_BY_PAGE(addr) (descriptors + ((addr) - (uint64_t)firstPage) / PAGE_SIZE)


extern MemMapEntry memMap[];
extern int memMapSize;


void initBuddy() {
	MemMapEntry freePlace = memMap[0];
	for (int i = 0; i < memMapSize; i++) {
		if (memMap[i].len > freePlace.len && memMap[i].type == 1)
			freePlace = memMap[i];
	}
	descriptors = (void*)LOGIC(freePlace.addr);
	

	// printf("%llx\n", PHYS(LOGIC(0x239)));

	while ((uint64_t)PHYS(descriptors) % 4 != 0)
		descriptors = (Descriptor*)(1 + (uint64_t)descriptors);


	while ((uint64_t)PHYS(descriptors) + 
		   (1ll << maxRank) * PAGE_SIZE + 
		   2 * (1ll << maxRank) * sizeof(Descriptor) 
		   < 
		   freePlace.addr + freePlace.len)
		++maxRank;

	--maxRank;

	firstPage = (void*)((uint64_t)descriptors +  2 * (1ll << maxRank) * sizeof(Descriptor));


	// printf("maxRank = %d\n", maxRank);

	for (int i = 0; i < 2 * (1 << maxRank) - 1; i++) {
		descriptors[i].next = descriptors[0].prev = NULL;
		descriptors[i].rank = 0;
		descriptors[i].free = 1;
	}

	for (int i = 0; i < maxRank; i++)
		rankHead[i] = NULL;
	rankHead[maxRank] = &descriptors[0];
	descriptors[0].rank = maxRank;

	printf("[OK] buddy allocator initialized\n");
}

void erase(Descriptor *d) {
	if (d->prev == NULL)
		rankHead[d->rank] = d->next;

	if (d->prev != NULL)
		d->prev->next = d->next;
	if (d->next != NULL)
		d->next->prev = d->prev;
}
void insert(int index, Descriptor *d) {
	Descriptor *oldHead = rankHead[index];
	rankHead[index] = d;
	d->prev = NULL;
	d->next = oldHead;
	if (d->next)
		d->next->prev = d;
}


void split(Descriptor *d) {
	erase(d);
	d->rank--;
	insert(d->rank, d);
	Descriptor *buddy = BUDDY(d);
	insert(d->rank, buddy);
	buddy->rank = d->rank;
	buddy->free = d->free;
}

Descriptor* IWant(int rank) {
	if (rank > maxRank)
		return NULL;
	if (rankHead[rank] != NULL) {
		// printf("ok\n");
		return rankHead[rank];
	}
	Descriptor *parent = IWant(rank + 1);
	if (parent == NULL)
		return NULL;
	split(parent);
	assert(rankHead != NULL);
	
	return rankHead[rank];
}

void* buddyAlloc(int rank) {
	Descriptor *d = IWant(rank);
	if (d == NULL) 
		return NULL;

	d->free = 0;
	erase(d);

	// printf("index = %d\n", INDEX(d));

	return PAGE(d);
}


void mergeWithBuddy(Descriptor *d) {
	
	if (d->rank >= maxRank)
		return;

	Descriptor *buddy = BUDDY(d);
	if (!buddy->free || buddy->rank != d->rank)
		return;

	erase(d);
	erase(buddy);

	Descriptor *parent = INDEX(d) < INDEX(buddy) ? d : buddy;
	parent->rank++;
	insert(parent->rank, parent);
	mergeWithBuddy(parent);
}


void buddyFree(void *addr) {
	Descriptor *d = DESCR_BY_PAGE((uint64_t)addr);
	// printf("free index = %d\n", INDEX(d));
	d->free = 1;
	insert(d->rank, d);
	mergeWithBuddy(d);
}	