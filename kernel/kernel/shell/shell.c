#include <kernel/shell.h>

#include <devices/keyboard.h>
#include <devices/terminal.h>
#include <stdio.h>

#include <stdbool.h>

void init_shell() {
    unsigned char command[4096];
    size_t cursor;
    while (true) {
        key_event_t ev;
        if (isKeyEvent()) {
            ev = getKeyEvent();
            if (isAscii(ev.key) && ev.key.press) {
                putchar(keyEventToAscii(ev));
            }
        }
    }
}