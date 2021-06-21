#ifndef VMM_H
#define VMM_H

#include "memory.h"

void initPaging();
void map(uint64_t vaddr, uint64_t paddr, size_t size, Perms perm, MemoryType mt);
Decoded decode(uint64_t entry, bool bottom_level);

Vaddr toVaddr(Paddr paddr);
Paddr toPaddr(Vaddr vaddr);

#define identity_map(addr, args...)     \
    do {                                \
        typeof(addr) address = addr;    \
        map(address, address, args);    \
    } while (0)

#endif // VMM_H