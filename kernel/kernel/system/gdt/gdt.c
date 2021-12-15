#define MODULE gdt

#include <kernel/system/gdt.h>

#include <kernel/kinfo.h>

#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) GdtPtr;

extern void reloadSegments(void);
extern void loadGdt(GdtPtr *gdtPtr);

uint64_t gdt[3] = {
    [0] = 0x000F00000000FFFF, // Null
    [1] = 0x00A09A0000000000, // 64 bit code
    [2] = 0x0000920000000000  // 64 bit data
};

GdtPtr gdtPtr = (GdtPtr) {
    .limit = sizeof(gdt) - 1,
    .base = (uint64_t)&gdt[0]
};


void initGdt(void) {
    loadGdt(&gdtPtr);
    reloadSegments();

    kinfoLog(Log_Success, "GDT loaded.");
}