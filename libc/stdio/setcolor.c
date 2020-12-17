#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

void setcolor(uint8_t color) {
#if defined(__is_libk)
	terminal_setcolor(color);
#endif
}