#include <kernel/system/idt.h>
#include "idt_internal.h"
#include <kernel/system/exceptions.h>
#include <sys/io.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

static void remapPic();
static void setExceptionEntries();
static void setIrqEntries();
static inline void setIdtEntry(uint8_t index, uint64_t irq_address);
static inline IdtPtr getIdtPtr();

IdtEntry idt[256];

void idtInit() {
    remapPic();

    setExceptionEntries();
    setIrqEntries();
    IdtPtr idt_ptr = getIdtPtr();
    asm volatile ("lidt %0" :: "m" (idt_ptr) : "memory");
    asm volatile ("sti");

    klog_info(KLOG_SUCCESS, "IDT loaded.");
}

static void remapPic() {
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

    klog_info(KLOG_SUCCESS, "PIC remapped.");
}

static void setExceptionEntries() {
    setIdtEntry(0,  (uint64_t)exc0);
    setIdtEntry(1,  (uint64_t)exc1);
    setIdtEntry(2,  (uint64_t)exc2);
    setIdtEntry(3,  (uint64_t)exc3);
    setIdtEntry(4,  (uint64_t)exc4);
    setIdtEntry(5,  (uint64_t)exc5);
    setIdtEntry(6,  (uint64_t)exc6);
    setIdtEntry(7,  (uint64_t)exc7);
    setIdtEntry(8,  (uint64_t)exc8);
    setIdtEntry(9,  (uint64_t)exc9);
    setIdtEntry(10, (uint64_t)exc10);
    setIdtEntry(11, (uint64_t)exc11);
    setIdtEntry(12, (uint64_t)exc12);
    setIdtEntry(13, (uint64_t)exc13);
    setIdtEntry(14, (uint64_t)exc14);
    setIdtEntry(15, (uint64_t)exc15);
    setIdtEntry(16, (uint64_t)exc16);
    setIdtEntry(17, (uint64_t)exc17);
    setIdtEntry(18, (uint64_t)exc18);
    setIdtEntry(19, (uint64_t)exc19);
    setIdtEntry(20, (uint64_t)exc20);
    setIdtEntry(30, (uint64_t)exc30);
}

static void setIrqEntries() {
    setIdtEntry(32, (uint64_t)irq0);
    setIdtEntry(33, (uint64_t)irq1);
    setIdtEntry(34, (uint64_t)irq2);
    setIdtEntry(35, (uint64_t)irq3);
    setIdtEntry(36, (uint64_t)irq4);
    setIdtEntry(37, (uint64_t)irq5);
    setIdtEntry(38, (uint64_t)irq6);
    setIdtEntry(39, (uint64_t)irq7);
    setIdtEntry(40, (uint64_t)irq8);
    setIdtEntry(41, (uint64_t)irq9);
    setIdtEntry(42, (uint64_t)irq10);
    setIdtEntry(43, (uint64_t)irq11);
    setIdtEntry(44, (uint64_t)irq12);
    setIdtEntry(45, (uint64_t)irq13);
    setIdtEntry(46, (uint64_t)irq14);
    setIdtEntry(47, (uint64_t)irq15);
}

static inline void setIdtEntry(uint8_t index, uint64_t irq_address) {
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