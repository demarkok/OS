#include <stdint.h>

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__


#define masterCommandPort 0x20
#define masterDataPort 0x21
#define slaveCommandPort 0xA0
#define slaveDataPort 0xA1

#define masterFirstIntId 32
#define slaveFirstIntId 40



void initIdt();
void initPic();

typedef struct idtEntry {
	uint16_t offset1;
	uint16_t segSelector;
	uint8_t _trash1;
	uint8_t typeAndP;
	uint16_t offset2;
	uint32_t offset3;
	uint32_t _trash2;
} __attribute__((packed)) idtEntry;



#endif /* __INTERRUPTS_H__ */