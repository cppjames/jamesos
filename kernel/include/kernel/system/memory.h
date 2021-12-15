#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uintptr_t Vaddr;
typedef uintptr_t Paddr;

typedef uint64_t Entry;
typedef Entry *Table;

typedef enum Perms {
    Perm_None = 0b000,
    Perm_X    = 0b001,
    Perm_W    = 0b010,
    Perm_R    = 0b100,

    Perm_RX   = 0b011,
    Perm_RW   = 0b110,
    Perm_RWX  = 0b111
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

extern const uint64_t higherBase;
extern const uint64_t kernelBase;

// TODO - Move these somewhere else
#define KiB(N) ((uint64_t)(N) << 10)
#define MiB(N) ((uint64_t)(N) << 20)
#define GiB(N) ((uint64_t)(N) << 30)
#define bit(N) (1UL << (N))
#define firstBits(N) (bit(N) - 1UL)

#define PAGE_SIZE KiB(4)


inline uint64_t alignUp(uint64_t addr, size_t size) {
    return (addr + (size - 1)) & ~(size - 1);
}


inline uint64_t alignDown(uint64_t addr, size_t size) {
    return (addr / size) * size;
}


#endif // MEMORY_H