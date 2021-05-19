#include <devices/keyboard.h>
#include <sys/io.h>
#include <stdio.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

// PIT
void irq0_handler() {
    // Nothin hia
}

// Keyboard
void irq1_handler() {
    unsigned char scan_code = inb(0x60);

    Key key = codeToKey(scan_code);
    if (key.code == KeyCode_None)
        return;

    sendKey(key);

    if (key.code == KeyCode_Pause) {
        Key key_release = key;
        key_release.press = false;
        sendKey(key_release);
    }
}

void irq2_handler()  { }
void irq3_handler()  { }
void irq4_handler()  { }
void irq5_handler()  { }
void irq6_handler()  { }
void irq7_handler()  { }
void irq8_handler()  { }
void irq9_handler()  { }
void irq10_handler() { }
void irq11_handler() { }
void irq12_handler() { }
void irq13_handler() { }
void irq14_handler() { }
void irq15_handler() { }