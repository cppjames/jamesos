#include <kernel/kcontrol.h>


void khalt(void) {
    asm ("hlt");
}