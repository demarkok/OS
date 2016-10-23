#include "commonTools.h"
#include "printf.h"
#include <stdint.h>

uint64_t submask(uint64_t x, int l, int h) {
	uint64_t filter = (h >= 63) ? (((uint64_t)0) - 1) : (~((~(uint64_t)0) << (h + 1)));
	return (filter & x) >> l;
}

void assert(int expr) {
	if (!expr) {
		printf("[ERROR] Assertion failed\n");
		while (1);
	}
}