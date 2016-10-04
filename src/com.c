#include "com.h"
#include "ioport.h"
#include <stdint.h>

#define SET_SPEED 0b10000011
#define DISABLE_INT 0b00000011

void initCOM() {

	out8(START + 3, SET_SPEED);

	// Div = 1
	out8(START + 0, 1);
	out8(START + 1, 0);

	out8(START + 3, DISABLE_INT);

	out8(START + 1, 0); // disable interruptions

	writeStr("[OK] serial port initialized\n");
} 

void writeByte(uint8_t byte) {
	unsigned short s = START;
	while (!(in8(s + 5) & ((uint8_t)1 << 5)));
	out8(s + 0, byte);
}

void writeStr(char *s) {
	for (char *c = s; *c; c++) 
		writeByte(*c);
}
