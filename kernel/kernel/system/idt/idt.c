#include <kernel/system/idt.h>
#include "idt_internal.h"
#include <kernel/system/exceptions.h>
#include <sys/io.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

static void remapPic();
static void setExceptionEntries();
static void setIrqEntries();
static inline void setIdtEntry(uint8_t index, void *irq_ptr);
static inline IdtPtr getIdtPtr();

IdtEntry idt[256];


void initIdt() {
    remapPic();

    setExceptionEntries();
    setIrqEntries();
    IdtPtr idt_ptr = getIdtPtr();
    asm volatile ("lidt %0" :: "m" (idt_ptr) : "memory");
    asm volatile ("sti");

    kinfoLog(Log_Success, "IDT loaded.");
}


static void remapPic() {
    // TODO - Maybe have these in 2 arrays?

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    kinfoLog(Log_Success, "PIC remapped.");
}


static inline void setIdtEntry(uint8_t index, void *irq_ptr) {
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


static inline IdtPtr getIdtPtr() {
    return (IdtPtr) {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };
}


// TODO - Ugh. Do something about this

static void setExceptionEntries() {
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


static void setIrqEntries() {
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