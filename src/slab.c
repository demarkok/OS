#include "slab.h"
#include "buddy.h"
#include "printf.h"
#include <stddef.h>

SlabAllocator createSlabAllocator(uint64_t blockSize, int maxQuantity) {	
	uint64_t rank = 0;
	while ((uint64_t)PAGE_SIZE * (1ll << rank) < (blockSize + sizeof(SlabDescriptor)) * (uint64_t)maxQuantity)
		++rank;
	// printf("RANK = %d\n", rank);
	SlabDescriptor *result = buddyAlloc(rank);
	// printf("addr = %llx\n", result);

	SlabDescriptor *it = result;
	for (int i = 0; i < maxQuantity - 1; i++) {
		// printf("it = %llx\n", it);
		SlabDescriptor *next = (SlabDescriptor*)((uint64_t)(it) + blockSize + sizeof(SlabDescriptor));
		it->next = next;
		it = next;
	}
	it->next = NULL;

	SlabAllocator allocator;
	allocator.head = result;
	allocator.start = result;
	return allocator;
}

void destroySlabAllocator(SlabAllocator allocator) {
	buddyFree(allocator.start);
}

void* slabAlloc(SlabAllocator *allocator) {
	if (allocator->head == NULL)
		return NULL;
	SlabDescriptor *result = allocator->head;
	allocator->head = allocator->head->next;
	return (void*)((uint64_t)result + sizeof(SlabDescriptor));
}

void slabFree(SlabAllocator* allocator, void *addr) {
	SlabDescriptor *newHead = (SlabDescriptor*)((uint64_t)addr - sizeof(SlabDescriptor));	
	newHead->next = allocator->head;	
	allocator->head = newHead;
}