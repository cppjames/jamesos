#include <devices/terminal.h>
#include <kernel/kinfo.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static inline void terminalPutcharAt(size_t x, size_t y, uint8_t c);
static inline void terminalPutEntryAt(size_t x, size_t y, uint16_t entry);
static inline uint16_t terminalEntryAt(size_t x, size_t y);
static inline void terminalAdvance();
static inline void terminalNewline();
static inline void terminalScroll();
static inline size_t getIndex(size_t x, size_t y);

static const size_t VGA_WIDTH  = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row    = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;


void initTerminal() {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vgaEntryColor(VgaColor_LightGray, VgaColor_Black);
	terminal_buffer = VGA_MEMORY;
    terminalClear();

    kinfoLog(Log_Success, "Terminal initialized.");
}


void terminalClear() {
    terminalFill(0, 0, VGA_WIDTH, VGA_HEIGHT, ' ');
}


void terminalFill(size_t x, size_t y, size_t w, size_t h, uint8_t c) {
	for (size_t row = y; row < h; row++)
		for (size_t col = x; col < w; col++)
			terminalPutcharAt(col, row, c);
}


void terminalWrite(const char *data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminalPutchar(data[i]);
}


void terminalWriteString(const char *data) {
    for (const char *str = data; *str; str++)
        terminalPutchar(*str);
}


void terminalPutchar(uint8_t c) {
    if (!c) return;

	if (c == '\n') {
        terminalNewline();
        return;
    }
    
	terminalPutcharAt(terminal_column, terminal_row, c);
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
    
    uint16_t entry = vgaEntry(' ', vgaEntryColor(VgaColor_LightGray, VgaColor_Black));
    for (size_t col = 0; col < VGA_WIDTH; col++)
        terminalPutEntryAt(col, VGA_HEIGHT-1, entry);
}


static inline void terminalPutcharAt(size_t x, size_t y, uint8_t c) {
	terminalPutEntryAt(x, y, vgaEntry(c, terminal_color));
}


static inline void terminalPutEntryAt(size_t x, size_t y, uint16_t entry) {
	terminal_buffer[getIndex(x, y)] = entry;
}


static inline uint16_t terminalEntryAt(size_t x, size_t y) {
    return terminal_buffer[getIndex(x, y)];
}


static inline size_t getIndex(size_t x, size_t y) {
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