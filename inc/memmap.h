#ifndef __MEMMAP_H__
#define __MEMMAP_H__

#include <stdint.h>

typedef struct MemMapEntry {
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) MemMapEntry;


void initMemoryMap();
void printMemoryMap();

#endif /*__MEMMAP_H__*/