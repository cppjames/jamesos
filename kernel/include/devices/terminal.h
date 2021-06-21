#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stddef.h>
#include <stdint.h>

typedef enum VgaColor {
	VgaColor_Black = 		0,
	VgaColor_Blue = 		1,
	VgaColor_Green = 		2,
	VgaColor_Cyan = 		3,
	VgaColor_Red = 			4,
	VgaColor_Magenta = 		5,
	VgaColor_Brown = 		6,
	VgaColor_LightGray = 	7,
	VgaColor_DarkGray = 	8,
	VgaColor_LightBlue = 	9,
	VgaColor_LightGreen = 	10,
	VgaColor_LightCyan = 	11,
	VgaColor_LightRed = 	12,
	VgaColor_LightMagenta = 13,
	VgaColor_LightBrown = 	14,
	VgaColor_White = 		15,
} VgaColor;

static inline uint8_t vgaEntryColor(VgaColor fg, VgaColor bg) {
	return fg | (bg << 4);
}

static inline uint16_t vgaEntry(uint8_t uc, uint8_t color) {
	return (uint16_t)uc | ((uint16_t)color << 8);
}

void initTerminal();

void terminalClear();
void terminalFill(size_t x, size_t y, size_t w, size_t h, uint8_t c);

void terminalPutchar(uint8_t c);
void terminalWrite(const char* data, size_t size);
void terminalWriteString(const char* data);

void terminalSetColor(uint8_t color);
void terminalMoveCursor(size_t x, size_t y);
size_t terminalCursorX();
size_t terminalCursorY();

#endif