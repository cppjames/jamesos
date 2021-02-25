#include <stdio.h>
#include <stdint.h>
#include <devices/terminal.h>

void setcolor(enum VgaColor color) {
#if defined(__is_libk)
	terminal_setcolor(vga_entry_color(color, VgaColor_Black));
#endif
}