#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

void terminal_initialize(void);

void terminal_clear();
void terminal_fill(size_t x, size_t y, size_t w, size_t h, uint8_t c);

void terminal_putchar(uint8_t c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

void terminal_setcolor(uint8_t color);


#endif
