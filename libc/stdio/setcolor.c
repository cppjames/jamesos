#include <stdio.h>
#include <stdint.h>
#include <devices/terminal.h>

void setcolor(enum vga_color color) {
#if defined(__is_libk)
	terminal_setcolor(vga_entry_color(color, VGA_COLOR_BLACK));
#endif
}