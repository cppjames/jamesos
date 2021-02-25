#include <devices/keyboard.h>
#include <sys/io.h>
#include <stdio.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

#define eoi_pic_master()            \
    outb(0x20, 0x20);

#define eoi_pic_slave() do {        \
    outb(0xA0, 0x20);               \
    outb(0x20, 0x20); } while (0)

// PIT
void irq0_handler(void) {
    eoi_pic_master();
}

// Keyboard
void irq1_handler(void) {
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

void irq2_handler(void)  { }
void irq3_handler(void)  { }
void irq4_handler(void)  { }
void irq5_handler(void)  { }
void irq6_handler(void)  { }
void irq7_handler(void)  { }
void irq8_handler(void)  { }
void irq9_handler(void)  { }
void irq10_handler(void) { }
void irq11_handler(void) { }
void irq12_handler(void) { }
void irq13_handler(void) { }
void irq14_handler(void) { }
void irq15_handler(void) { }