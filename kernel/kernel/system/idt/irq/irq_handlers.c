#include <devices/keyboard.h>
#include <sys/io.h>
#include <stdio.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

// TODO - This file is a complete mess


// PIT
void irqHandler0() {
    // Context switch ?????????????
    // Scheduler shit ?????????????
}


// Keyboard
void irqHandler1() {
    unsigned char scan_code = inb(KEYBOARD_PORT);

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


void irqHandler2()  { }
void irqHandler3()  { }
void irqHandler4()  { }
void irqHandler5()  { }
void irqHandler6()  { }
void irqHandler7()  { }
void irqHandler8()  { }
void irqHandler9()  { }
void irqHandler10() { }
void irqHandler11() { }
void irqHandler12() { }
void irqHandler13() { }
void irqHandler14() { }
void irqHandler15() { }