#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <devices/terminal.h>

static inline void terminal_putc_at(size_t x, size_t y, uint8_t c);
static inline void terminal_putentryat(size_t x, size_t y, uint16_t entry);
static inline uint16_t terminal_entryat(size_t x, size_t y);
static inline void terminal_advance(void);
static inline void terminal_newline(void);
static inline void terminal_scroll(void);
static inline size_t get_index(size_t x, size_t y);

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t *terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;

    terminal_clear();
}

void terminal_clear() {
    terminal_fill(0, 0, VGA_WIDTH, VGA_HEIGHT, ' ');
}

void terminal_fill(size_t x, size_t y, size_t w, size_t h, uint8_t c) {
	for (size_t i = y; i < h; i++)
		for (size_t j = x; j < w; j++)
			terminal_putc_at(j, i, c);
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    // TODO: This should be a while loop
	terminal_write(data, strlen(data));
}

void terminal_putchar(uint8_t c) {
	if (c == '\n') {
        terminal_newline();
        return;
    }
	terminal_putc_at(terminal_column, terminal_row, c);
	terminal_advance();
}

void terminal_advance(void) {
    if (++terminal_column == VGA_WIDTH)
        terminal_newline();
}

void terminal_newline(void) {
    terminal_column = 0;
    
    if (terminal_row < VGA_HEIGHT-1)
        ++terminal_row;
    else
        terminal_scroll();
}

static void terminal_scroll(void) {
    // TODO: Implement using memmove
    for (size_t y = 0; y < VGA_HEIGHT-1; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            terminal_putentryat(x, y, terminal_entryat(x, y+1));
    
    // TODO: Looks ugly. Maybe terminal_fill_line?
    uint16_t entry = vga_entry(' ', vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    for (size_t x = 0; x < VGA_WIDTH; x++)
        terminal_putentryat(x, VGA_HEIGHT-1, entry);
}

static inline void terminal_putc_at(size_t x, size_t y, uint8_t c) {
	terminal_putentryat(x, y, vga_entry(c, terminal_color));
}
static inline void terminal_putentryat(size_t x, size_t y, uint16_t entry) {
	terminal_buffer[get_index(x, y)] = entry;
}
static inline uint16_t terminal_entryat(size_t x, size_t y) {
    return terminal_buffer[get_index(x, y)];
}

static inline size_t get_index(size_t x, size_t y) {
    return y * VGA_WIDTH + x;
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}