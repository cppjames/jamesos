#ifndef VMM_H
#define VMM_H

#include "memory.h"

void initPaging();
void map(Vaddr vaddr, Paddr paddr, size_t size, Perms perm, MemoryType mt);
Decoded decode(Entry entry, bool bottom_level);

Vaddr toVaddr(Paddr paddr);
Paddr toPaddr(Vaddr vaddr);

#define identity_map(addr, args...)     \
    do {                                \
        typeof(addr) address = addr;    \
        map(address, address, args);    \
    } while (0)

#endif // VMM_H