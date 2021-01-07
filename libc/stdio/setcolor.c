#include <stdio.h>
#include <stdint.h>
#include <vga.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

void setcolor(enum vga_color color) {
#if defined(__is_libk)
	terminal_setcolor(vga_entry_color(color, VGA_COLOR_BLACK));
#endif
}