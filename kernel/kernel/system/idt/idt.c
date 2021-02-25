#include <kernel/system/idt.h>
#include "idt_internal.h"
#include <kernel/system/exceptions.h>
#include <sys/io.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

static void remap_pic();
static void set_exception_entries();
static void set_irq_entries();
static inline void set_idt_entry(uint8_t, uint64_t);
static inline IdtPtr get_idt_ptr();

IdtEntry idt[256];

void init_idt() {
    remap_pic();

    set_exception_entries();
    set_irq_entries();
    IdtPtr idt_ptr = get_idt_ptr();
    asm volatile ("lidt %0" :: "m" (idt_ptr) : "memory");
    asm volatile ("sti");

    klog_info(KLOG_SUCCESS, "IDT loaded.");
}

static void remap_pic() {
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

static void set_exception_entries() {
    set_idt_entry(0,  (uint64_t)exc0);
    set_idt_entry(1,  (uint64_t)exc1);
    set_idt_entry(2,  (uint64_t)exc2);
    set_idt_entry(3,  (uint64_t)exc3);
    set_idt_entry(4,  (uint64_t)exc4);
    set_idt_entry(5,  (uint64_t)exc5);
    set_idt_entry(6,  (uint64_t)exc6);
    set_idt_entry(7,  (uint64_t)exc7);
    set_idt_entry(8,  (uint64_t)exc8);
    set_idt_entry(9,  (uint64_t)exc9);
    set_idt_entry(10, (uint64_t)exc10);
    set_idt_entry(11, (uint64_t)exc11);
    set_idt_entry(12, (uint64_t)exc12);
    set_idt_entry(13, (uint64_t)exc13);
    set_idt_entry(14, (uint64_t)exc14);
    set_idt_entry(15, (uint64_t)exc15);
    set_idt_entry(16, (uint64_t)exc16);
    set_idt_entry(17, (uint64_t)exc17);
    set_idt_entry(18, (uint64_t)exc18);
    set_idt_entry(19, (uint64_t)exc19);
    set_idt_entry(20, (uint64_t)exc20);
    set_idt_entry(30, (uint64_t)exc30);
}

static void set_irq_entries() {
    set_idt_entry(32, (uint64_t)irq0);
    set_idt_entry(33, (uint64_t)irq1);
    set_idt_entry(34, (uint64_t)irq2);
    set_idt_entry(35, (uint64_t)irq3);
    set_idt_entry(36, (uint64_t)irq4);
    set_idt_entry(37, (uint64_t)irq5);
    set_idt_entry(38, (uint64_t)irq6);
    set_idt_entry(39, (uint64_t)irq7);
    set_idt_entry(40, (uint64_t)irq8);
    set_idt_entry(41, (uint64_t)irq9);
    set_idt_entry(42, (uint64_t)irq10);
    set_idt_entry(43, (uint64_t)irq11);
    set_idt_entry(44, (uint64_t)irq12);
    set_idt_entry(45, (uint64_t)irq13);
    set_idt_entry(46, (uint64_t)irq14);
    set_idt_entry(47, (uint64_t)irq15);
}

static inline void set_idt_entry(uint8_t index, uint64_t irq_address) {
    idt[index] = (IdtEntry) {
        .offset_lowerbits =   irq_address & OFFSET_MASK_LOWER,
        .offset_middlebits = (irq_address & OFFSET_MASK_MIDDLE) >> 16,
        .offset_higherbits = (irq_address & OFFSET_MASK_HIGHER) >> 32,

        .zero_1 = 0,
        .zero_2 = 0,

        .selector = KERNEL_CODE_SEGMENT_OFFSET,
        .type_attr = INTERRUPT_GATE
    };
}

static inline IdtPtr get_idt_ptr() {
    return (IdtPtr) {
        .limit = sizeof(idt) - 1,
        .base = (uint64_t)idt
    };
}