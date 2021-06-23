#include <devices/terminal.h>

#include <kernel/kinfo.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

static uint16_t* const vga_memory = (uint16_t*)0xB8000;

static size_t terminal_row    = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

static void terminalPutcharAt(size_t x, size_t y, uint8_t ch);
static void terminalPutEntryAt(size_t x, size_t y, uint16_t entry);
static uint16_t terminalEntryAt(size_t x, size_t y);
static void terminalAdvance();
static void terminalNewline();
static void terminalScroll();
static size_t getIndex(size_t x, size_t y);


void initTerminal() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vgaColorEntry(VgaColor_LightGray, VgaColor_Black);
	terminal_buffer = vga_memory;
    terminalClear();

    kinfoLog(Log_Success, "Terminal initialized.");
}


void terminalClear() {
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
    if (!ch) return;

	if (ch == '\n') {
        terminalNewline();
        return;
    }
    
	terminalPutcharAt(terminal_column, terminal_row, ch);
	terminalAdvance();
}


void terminalAdvance() {
    if (++terminal_column == VGA_WIDTH)
        terminalNewline();
}


void terminalNewline() {
    terminal_column = 0;
    
    if (terminal_row < VGA_HEIGHT-1)
        ++terminal_row;
    else
        terminalScroll();
}


static void terminalScroll() {
    for (size_t row = 0; row < VGA_HEIGHT-1; row++)
        for (size_t col = 0; col < VGA_WIDTH; col++)
            terminalPutEntryAt(col, row, terminalEntryAt(col, row + 1));
    
    VgaEntry entry = vgaEntry(' ', vgaColorEntry(VgaColor_LightGray, VgaColor_Black));
    for (size_t col = 0; col < VGA_WIDTH; col++)
        terminalPutEntryAt(col, VGA_HEIGHT-1, entry);
}


static void terminalPutcharAt(size_t x, size_t y, uint8_t ch) {
	terminalPutEntryAt(x, y, vgaEntry(ch, terminal_color));
}


static void terminalPutEntryAt(size_t x, size_t y, VgaEntry entry) {
	terminal_buffer[getIndex(x, y)] = entry;
}


static VgaEntry terminalEntryAt(size_t x, size_t y) {
    return terminal_buffer[getIndex(x, y)];
}


static size_t getIndex(size_t x, size_t y) {
    return y * VGA_WIDTH + x;
}


void terminalSetColor(uint8_t color) {
	terminal_color = color;
}


void terminalMoveCursor(size_t x, size_t y) {
    terminal_column = x;
    terminal_row = y;
}


size_t terminalCursorX() {
    return terminal_column;
}


size_t terminalCursorY() {
    return terminal_row;
}