#include <kernel/kdebug.h>
#include <sys/io.h>
#include <devices/keyboard.h>
#include <stdint.h>
#include <stdbool.h>

// Hexdump implementation template for 16, 32 and 64 bits.
#define HEXDUMP_IMPL(SIZE)                                      \
    void kdebug_hexdump ## SIZE (void *address, size_t count) { \
        uint ## SIZE ## _t type;                                \
        size_t width = sizeof(type) * 2;                        \
        for (size_t idx = 0; idx < count; idx++) {              \
            size_t mem = (size_t)address + idx * sizeof(type);  \
            klog_debug("%0*zX\n", width, *(typeof(type)*)mem);  \
        }                                                       \
    }

HEXDUMP_IMPL(64)
HEXDUMP_IMPL(32)
HEXDUMP_IMPL(16)

void printch_port(char ch, __attribute__((unused)) void* arg) {
    outb(0xE9, (uint8_t)ch);
}

void _kbreak() {
    while (true) {
        KeyEvent ev = getKeyEvent();
        if ((ev.key.code == KeyCode_Space || ev.key.code == KeyCode_Return) && ev.key.press)
            return;
    }
}