#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef enum Perms {
    Perm_None = 0b000,
    Perm_X =    0b001,
    Perm_W =    0b010,
    Perm_R =    0b100,

    Perm_RX =   0b011,
    Perm_RW =   0b110,
    Perm_RWX =  0b111
} Perms;

typedef enum MemoryType {
    MemoryType_Memory,
    MemoryType_MMIO
} MemoryType;

void init_paging();
void map(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt);

void *kallocFrame();
void kfreeFrame(void *frame);

#endif // MEMORY_H