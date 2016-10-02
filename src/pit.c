#include "pit.h"
#include "commonTools.h"
#include "ioport.h"
#include "interrupts.h"
#include "com.h"
#include "interrupts.h"
#include <stdint.h>

#define COMMAND_PORT 0x43
#define DATA_PORT 0x40
#define INIT_CNT (1193182 / FREQ)
#define FREQ 41

void tick() {
	static int cnt = FREQ;
	--cnt;
	if (cnt == 0) {
		writeStr("tick\n");
		cnt = 41;
	}
}

void initPit() { // FREQUENCY = 41 Hz
	out8(COMMAND_PORT, 0b00110100); // 00 1 1 010 0
	out8(DATA_PORT, (uint8_t)submask(INIT_CNT, 0, 7));
	out8(DATA_PORT, (uint8_t)submask(INIT_CNT, 8, 15));

	out8(masterDataPort, 0b11111110);
}