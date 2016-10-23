#include "memmap.h"

#include <stdint.h>
#include "printf.h"


extern uint8_t *toMemMap;

extern uint8_t text_phys_begin[];
extern uint8_t bss_phys_end[];


#define MEM_MAP_MAX_SIZE 10

MemMapEntry memMap[MEM_MAP_MAX_SIZE];
int memMapSize = 0;

void removeEntry(int index) {
	for (int i = index; i < memMapSize - 1; i++) {
		memMap[i] = memMap[i + 1];
	}
	memMapSize--;
}

void insertEntry(int index, MemMapEntry entry) { // insert after
	for (int i = index + 1; i <= memMapSize; i++) {
		MemMapEntry tmp = memMap[i];
		memMap[i] = entry;
		entry = tmp;
	}
	memMapSize++;
}

MemMapEntry cutPrefix(MemMapEntry x, uint64_t len) {
	x.len -= len;
	x.addr += len;
	return x;
}

MemMapEntry cutSuffix(MemMapEntry x, uint64_t len) {
	x.len -= len;
	return x;
}

uint64_t end(MemMapEntry x) {
	return x.addr + x.len - 1;
}
uint64_t beg(MemMapEntry x) {
	return x.addr;
}

void initMemoryMap() {

	MemMapEntry *map = (MemMapEntry*)((uint64_t)(*((uint32_t*)(toMemMap + 48))));
	int length = *((uint32_t*)(toMemMap + 44)); 

	uint8_t *ff = (uint8_t*)map;
	for (int i = 0; (uint8_t*)map - ff < length; i++) {

		memMap[i] = *map;
		
		map = (MemMapEntry*)(((uint8_t*)(map)) + map->size + 4);
		++memMapSize;
	}

	MemMapEntry kernel; 

	kernel.addr = (uint64_t)text_phys_begin;
	kernel.len = (uint64_t)(bss_phys_end - text_phys_begin + 1);
	kernel.type = 2;

	for (int i = 0; i < memMapSize; i++) {
		if (beg(memMap[i]) >= beg(kernel) && end(memMap[i]) <= end(kernel)) {
			removeEntry(i);
		}
	}

	int ok = 0;

	for (int i = 0; i < memMapSize; i++) {
		if (beg(memMap[i]) < beg(kernel) && end(memMap[i]) > end(kernel)) {
			MemMapEntry f = cutSuffix(memMap[i], end(memMap[i]) - beg(kernel) + 1);
			MemMapEntry s = cutPrefix(memMap[i], end(kernel) - beg(memMap[i]) + 1);
			insertEntry(i, f);
			insertEntry(i + 1, kernel);
			insertEntry(i + 2, s);
			removeEntry(i);
			ok = 1;
			break;
		}
	}

	if (!ok) {
		for (int i = 0; i < memMapSize; i++) {
			if (beg(kernel) > beg(memMap[i]) && beg(kernel) <= end(memMap[i])) {
				memMap[i] = cutSuffix(memMap[i], end(memMap[i]) - beg(kernel) + 1);
			}
			if (end(kernel) >= beg(memMap[i]) && end(kernel) < end(memMap[i])) {
				memMap[i] = cutPrefix(memMap[i], end(kernel) - beg(memMap[i]) + 1);
			}
		}
		int q = 0; 
		while (q < memMapSize && end(memMap[q]) < beg(kernel))
			++q;
		--q;
		insertEntry(q, kernel);
	}
	printf("[OK] memory map initialized\n");
}

void printMemoryMap() {

	for (int i = 0; i < memMapSize; i++) {
		printf("memory range: %llx - %llx (%lld B), type = %d\n", beg(memMap[i]), 
																  end(memMap[i]), 
																  memMap[i].len, 
																  memMap[i].type);			
	}

}