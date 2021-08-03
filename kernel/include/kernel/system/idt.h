#ifndef IDT_H
#define IDT_H

typedef enum IrqLine {
    Irq_PIT,
    Irq_KeyboardInterrupt,
    Irq_Cascade,
    Irq_COM2,
    Irq_COM1,
    Irq_LPT2,
    Irq_FloppyDisk,
    Irq_LPT1,
    Irq_CMOS,
    Irq_Free1,
    Irq_Free2,
    Irq_Free3,
    Irq_PS2Mouse,
    Irq_FPU,
    Irq_PrimaryATA,
    Irq_SecondaryATA
} IrqLine;

typedef void (*IrqHandler)(void);

void initIdt(void);

void registerIrqHandler(IrqLine line, IrqHandler handler);
void setIrqMask(IrqLine irqLine);
void clearIrqMask(IrqLine irqLine);

inline void clearInterrupts(void) {
    asm ("cli");
}

inline void setInterrupts(void) {
    asm ("sti");
}

#define eoiPicMaster()              \
    outb(0x20, 0x20);

#define eoiPicSlave() do {          \
    outb(0xA0, 0x20);               \
    outb(0x20, 0x20); } while (0)

#endif // IDT_H