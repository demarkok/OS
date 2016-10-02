#ifndef __COM_H__
#define __COM_H__

#include <stdint.h>

#define START 0x3f8

void initCOM();
void writeByte(uint8_t byte);
void writeStr(char *s);
void writeInt(uint64_t x);

#endif /*__COM_H__*/
