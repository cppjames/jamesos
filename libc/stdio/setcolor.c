#include <stdio.h>
#include <stdint.h>
#include <devices/terminal.h>

void setcolor(VgaColor color) {
#if defined(__is_libk)
	terminalSetColor(vgaEntryColor(color, VgaColor_Black));
#endif
}