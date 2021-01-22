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
    klog_debug("PIT handler has been called.\n");
    eoi_pic_master();
}

// Keyboard
void irq1_handler(void) {
    eoi_pic_master();
}
 
void irq2_handler(void) {
    eoi_pic_master();
}
 
void irq3_handler(void) {
    eoi_pic_master();
}
 
void irq4_handler(void) {
    eoi_pic_master();
}
 
void irq5_handler(void) {
    eoi_pic_master();
}
 
void irq6_handler(void) {
    eoi_pic_master();
}
 
void irq7_handler(void) {
    eoi_pic_master();
}
 
void irq8_handler(void) {
    eoi_pic_slave();
}
 
void irq9_handler(void) {
    eoi_pic_slave();
}
 
void irq10_handler(void) {
    eoi_pic_slave();
}
 
void irq11_handler(void) {
    eoi_pic_slave();
}
 
void irq12_handler(void) {
    eoi_pic_slave();
}
 
void irq13_handler(void) {
    eoi_pic_slave();
}
 
void irq14_handler(void) {
    eoi_pic_slave();
}
 
void irq15_handler(void) {
    eoi_pic_slave();
}