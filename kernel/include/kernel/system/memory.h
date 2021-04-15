#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

#define KiB(N) ((uint64_t)(N) << 10)
#define MiB(N) ((uint64_t)(N) << 20)
#define GiB(N) ((uint64_t)(N) << 30)

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

typedef enum Decoded {
    Decoded_Empty,
    Decoded_Mapping,
    Decoded_Table
} Decoded;

extern const uint64_t higher_base;
extern const uint64_t kernel_base;

void init_paging();
void map(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt);
Decoded decode(uint64_t entry, bool bottom_level);

#define identity_map(ADDR, ...) map(ADDR, ADDR, __VA_ARGS__);

void *kallocFrame();
void kfreeFrame(void *frame);

#endif // MEMORY_H