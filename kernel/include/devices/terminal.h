#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stddef.h>
#include <stdint.h>

enum VgaColor {
	VgaColor_Black = 0,
	VgaColor_Blue = 1,
	VgaColor_Green = 2,
	VgaColor_Cyan = 3,
	VgaColor_Red = 4,
	VgaColor_Magenta = 5,
	VgaColor_Brown = 6,
	VgaColor_LightGray = 7,
	VgaColor_DarkGray = 8,
	VgaColor_LightBlue = 9,
	VgaColor_LightGreen = 10,
	VgaColor_LightCyan = 11,
	VgaColor_LightRed = 12,
	VgaColor_LightMagenta = 13,
	VgaColor_LightBrown = 14,
	VgaColor_White = 15,
};

static inline uint8_t vga_entry_color(enum VgaColor fg, enum VgaColor bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(uint8_t uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void init_terminal(void);

void terminal_clear();
void terminal_fill(size_t x, size_t y, size_t w, size_t h, uint8_t c);

void terminal_putchar(uint8_t c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

void terminal_setcolor(uint8_t color);
void terminal_mvcursor(size_t x, size_t y);
size_t terminal_cursor_x();
size_t terminal_cursor_y();

#endif