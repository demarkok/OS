#ifndef __SLAB_H__
#define __SLAB_H__

#include <stdint.h>



typedef struct SlabDescriptor {
	struct SlabDescriptor *next;
} __attribute__((packed)) SlabDescriptor;

typedef struct SlabAllocator {
	void* start;
	SlabDescriptor *head;
} SlabAllocator;

SlabAllocator createSlabAllocator(uint64_t blockSize, int maxQuantity);
void destroySlabAllocator(SlabAllocator allocator);
void* slabAlloc(SlabAllocator* allocator);
void slabFree(SlabAllocator* allocator, void *addr);

#endif /*__SLAB_H__*/