#include <kernel/kcontrol.h>

void khalt() {
    asm ("hlt");
}