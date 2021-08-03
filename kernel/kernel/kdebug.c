#include <kernel/kdebug.h>

#include <devices/keyboard.h>

#include <stdbool.h>
#include <stdint.h>
#include <sys/io.h>

// Hexdump implementation template for 16, 32 and 64 bits.
#define HEXDUMP_IMPL(SIZE)                                      \
    void kdebugHexdump ## SIZE (void *address, size_t count) { \
        uint ## SIZE ## _t type;                                \
        size_t width = sizeof(type) * 2;                        \
        for (size_t idx = 0; idx < count; idx++) {              \
            size_t mem = (size_t)address + idx * sizeof(type);  \
            kdebugLog("%0*zX\n", width, *(typeof(type)*)mem);  \
        }                                                       \
    }

HEXDUMP_IMPL(64)
HEXDUMP_IMPL(32)
HEXDUMP_IMPL(16)


void kdebugPutchar(char ch, __attribute__((unused)) void* arg) {
    outb(0xE9, (uint8_t)ch);
}


void kbreakWaitKey(void) {
    while (true) {
        KeyEvent ev = getKeyEvent();
        if ((ev.key.code == KeyCode_Space || ev.key.code == KeyCode_Return) && ev.key.press)
            return;
    }
}