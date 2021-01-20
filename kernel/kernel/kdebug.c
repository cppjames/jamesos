#include <kernel/kdebug.h>
#include <sys/io.h>
#include <stdint.h>
#include <stdbool.h>

int printch_port(int ch) {
    outb(0xE9, (uint8_t)ch);
    return true;
}