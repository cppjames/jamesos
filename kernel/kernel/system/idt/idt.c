#define MODULE idt

#include <kernel/system/idt.h>
#include "idt_internal.h"
#include <kernel/system/exceptions.h>
#include <sys/io.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

static void remapPic(void);
static void setExceptionEntries(void);
static void setIrqEntries(void);
static inline void maskAllIrqs(void);
static inline void setIdtEntry(uint8_t index, void *irq_ptr);
static inline IdtPtr getIdtPtr(void);

IdtEntry idt[256];
IrqHandler irqHandlers[16];

void initIdt(void) {
    remapPic();

    setExceptionEntries();
    setIrqEntries();
    IdtPtr idt_ptr = getIdtPtr();
    asm volatile ("lidt %0" :: "m" (idt_ptr) : "memory");

    maskAllIrqs();
    asm volatile ("sti");

    kinfoLog(Log_Success, "IDT loaded.");
}


static void remapPic(void) {
    unsigned char ports[] =  { 0x20, 0xA0, 0x21, 0xA1, 0x21, 0xA1, 0x21, 0xA1, 0x21, 0xA1 };
    unsigned char values[] = { 0x11, 0x11, 0x20, 0x28, 0x04, 0x02, 0x01, 0x01, 0x00, 0x00 };

    size_t len = sizeof ports / sizeof ports[0];
    for (size_t i = 0; i < len; i++)
        outb(ports[i], values[i]);

    kinfoLog(Log_Success, "PIC remapped.");
}


void maskAllIrqs(void) {
    for (unsigned char line = 0; line < 16; line++)
        setIrqMask(line);
}


static void setIdtEntry(uint8_t index, void *irq_ptr) {
    uintptr_t irq_address = (uintptr_t)irq_ptr;

    idt[index] = (IdtEntry) {
        .offset_lowerbits =  (irq_address & OFFSET_MASK_LOWER),
        .offset_middlebits = (irq_address & OFFSET_MASK_MIDDLE) >> 16,
        .offset_higherbits = (irq_address & OFFSET_MASK_HIGHER) >> 32,

        .zero_1 = 0,
        .zero_2 = 0,

        .selector = KERNEL_CODE_SEGMENT_OFFSET,
        .type_attr = INTERRUPT_GATE
    };
}


static IdtPtr getIdtPtr(void) {
    return (IdtPtr) {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };
}


void registerIrqHandler(IrqLine line, IrqHandler handler) {
    irqHandlers[line] = handler;
}


void setIrqMask(IrqLine irqLine) {
    uint16_t port;
    uint8_t value;
 
    if(irqLine < 8) {
        // Master PIC data port
        port = 0x21;
    } else {
        // Slave PIC data port
        port = 0xA1;
        irqLine -= 8;
    }

    value = inb(port) | (1 << irqLine);
    outb(port, value);
}


void clearIrqMask(IrqLine irqLine) {
    uint16_t port;
    uint8_t value;
 
    if(irqLine < 8) {
        // Master PIC data port
        port = 0x21;
    } else {
        // Slave PIC data port
        port = 0xA1;
        irqLine -= 8;
    }
    
    value = inb(port) & ~(1 << irqLine);
    outb(port, value); 
}


// TODO - Ugh. Do something about this

static void setExceptionEntries(void) {
    setIdtEntry(0,  excIsr0);
    setIdtEntry(1,  excIsr1);
    setIdtEntry(2,  excIsr2);
    setIdtEntry(3,  excIsr3);
    setIdtEntry(4,  excIsr4);
    setIdtEntry(5,  excIsr5);
    setIdtEntry(6,  excIsr6);
    setIdtEntry(7,  excIsr7);
    setIdtEntry(8,  excIsr8);
    setIdtEntry(9,  excIsr9);
    setIdtEntry(10, excIsr10);
    setIdtEntry(11, excIsr11);
    setIdtEntry(12, excIsr12);
    setIdtEntry(13, excIsr13);
    setIdtEntry(14, excIsr14);
    setIdtEntry(15, excIsr15);
    setIdtEntry(16, excIsr16);
    setIdtEntry(17, excIsr17);
    setIdtEntry(18, excIsr18);
    setIdtEntry(19, excIsr19);
    setIdtEntry(20, excIsr20);
    setIdtEntry(30, excIsr30);
}


static void setIrqEntries(void) {
    setIdtEntry(32, irqIsr0);
    setIdtEntry(33, irqIsr1);
    setIdtEntry(34, irqIsr2);
    setIdtEntry(35, irqIsr3);
    setIdtEntry(36, irqIsr4);
    setIdtEntry(37, irqIsr5);
    setIdtEntry(38, irqIsr6);
    setIdtEntry(39, irqIsr7);
    setIdtEntry(40, irqIsr8);
    setIdtEntry(41, irqIsr9);
    setIdtEntry(42, irqIsr10);
    setIdtEntry(43, irqIsr11);
    setIdtEntry(44, irqIsr12);
    setIdtEntry(45, irqIsr13);
    setIdtEntry(46, irqIsr14);
    setIdtEntry(47, irqIsr15);
}