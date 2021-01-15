#ifndef _IO_H
#define _IO_H 1

#include <stdint.h>

unsigned char inb(unsigned short);
void outb(uint16_t, uint8_t);

#endif // _IO_H