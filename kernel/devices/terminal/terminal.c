#define MODULE term

#include <devices/terminal.h>

#include <kernel/kinfo.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

static void* const vgaMemory = (void*)0xB8000;

static size_t terminalRow = 0;
static size_t terminalColumn = 0;
static VgaColorEntry terminalColor;
static VgaEntry (*terminalBuffer)[VGA_HEIGHT][VGA_WIDTH] = vgaMemory;

static void terminalPutcharAt(size_t x, size_t y, uint8_t ch);
static void terminalPutEntryAt(size_t x, size_t y, VgaEntry entry);
static VgaEntry terminalEntryAt(size_t x, size_t y);
static void terminalAdvance(void);
static void terminalNewline(void);
static void terminalScroll(void);


void initTerminal(void) {
	terminalColor = vgaColorEntry(VgaColor_LightGray, VgaColor_Black);
    terminalClear();

    kinfoLog(Log_Success, "Terminal initialized.");
}


void terminalClear(void) {
    terminalFill(0, 0, VGA_WIDTH, VGA_HEIGHT, ' ');
}


void terminalFill(size_t x, size_t y, size_t w, size_t h, uint8_t ch) {
	for (size_t row = y; row < h; row++)
		for (size_t col = x; col < w; col++)
			terminalPutcharAt(col, row, ch);
}


void terminalWrite(const char *data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminalPutchar(data[i]);
}


void terminalWriteString(const char *data) {
    for (const char *str = data; *str; str++)
        terminalPutchar(*str);
}


void terminalPutchar(uint8_t ch) {
    if (ch == '\0')
        return;

    if (ch == '\n') {
        terminalNewline();
        return;
    }
    
	terminalPutcharAt(terminalColumn, terminalRow, ch);
	terminalAdvance();
}


void terminalAdvance(void) {
    if (++terminalColumn == VGA_WIDTH)
        terminalNewline();
}


void terminalNewline(void) {
    terminalColumn = 0;
    
    if (terminalRow < VGA_HEIGHT-1)
        ++terminalRow;
    else
        terminalScroll();
}


static void terminalScroll(void) {
    for (size_t row = 0; row < VGA_HEIGHT-1; row++)
        for (size_t col = 0; col < VGA_WIDTH; col++)
            terminalPutEntryAt(col, row, terminalEntryAt(col, row + 1));
    
    VgaEntry entry = vgaEntry(' ', vgaColorEntry(VgaColor_LightGray, VgaColor_Black));
    for (size_t col = 0; col < VGA_WIDTH; col++)
        terminalPutEntryAt(col, VGA_HEIGHT-1, entry);
}


static void terminalPutcharAt(size_t x, size_t y, uint8_t ch) {
	terminalPutEntryAt(x, y, vgaEntry(ch, terminalColor));
}


static void terminalPutEntryAt(size_t x, size_t y, VgaEntry entry) {
	(*terminalBuffer)[y][x] = entry;
}


static VgaEntry terminalEntryAt(size_t x, size_t y) {
    return (*terminalBuffer)[y][x];
}


void terminalSetColor(uint8_t color) {
	terminalColor = color;
}


void terminalMoveCursor(size_t x, size_t y) {
    terminalColumn = x;
    terminalRow = y;
}


size_t terminalCursorX(void) {
    return terminalColumn;
}


size_t terminalCursorY(void) {
    return terminalRow;
}