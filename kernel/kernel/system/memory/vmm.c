#define MODULE vmm

#include <kernel/system/vmm.h>
#include <kernel/system/pmm.h>

#include <kernel/kcontrol.h>
#include <kernel/kdebug.h>
#include <kernel/kinfo.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ENTRY_COUNT 512UL
#define ADDR_MASK   ~firstBits(entryMaskBits(0))

static size_t mapLargest(Vaddr vaddr, Paddr paddr, size_t size, Perms perm, MemoryType mt);
static bool canMap(size_t size, Vaddr vaddr, Paddr paddr, size_t pageSize);

static uint64_t extraBits(Perms perm, MemoryType mt, uint64_t level, bool mapping);
static size_t getIndex(Vaddr vaddr, uint64_t level);
static size_t entryMaskBits(uint64_t level);

static size_t vaddrBaseOffset(Vaddr vaddr, uint64_t level);
static Paddr entryToAddress(Entry entry);

static uint64_t *makeTableAt(Entry *entry, uint64_t bits);
inline static void makeMappingAt(Entry *entry, Paddr paddr, uint64_t bits);

extern void enableNXE(void);
extern void loadTable(Table table);

Table rootTable = NULL;
const uint64_t levels = 4;

const Vaddr higherBase = 0xFFFF800000000000;
const Vaddr kernelBase = 0xFFFFFFFF80000000;


void initPaging(void) {
    initPmm();

    rootTable = (Table)kallocFrame();
    identityMap(0x0, GiB(5), Perm_RWX, MemoryType_Memory);
    map(higherBase, 0x0, GiB(5), Perm_RWX, MemoryType_Memory);
    map(kernelBase, 0x0, GiB(2), Perm_RWX, MemoryType_Memory);
    
    enableNXE();
    loadTable((Table)toPaddr((Vaddr)rootTable));

    kinfoLog(Log_Success, "Paging enabled.");
}


void map(Vaddr vaddr, Paddr paddr, size_t size, Perms perm, MemoryType mt) {
    size += firstBits(entryMaskBits(0));
    size &= ~firstBits(entryMaskBits(0));

    while (size) {
        size_t maxMapSize = mapLargest(vaddr, paddr, size, perm, mt);

        vaddr += maxMapSize;
        paddr += maxMapSize;
        size -= maxMapSize;
    }
}


static size_t mapLargest(Vaddr vaddr, Paddr paddr, size_t size, Perms perm, MemoryType mt) {
    uint64_t level = levels - 1;
    
    size_t currentStepSize = bit(entryMaskBits(level));
    Table currentTable = rootTable;

    while (true) {
        const uint64_t mapBits = extraBits(perm, mt, level, true);
        const uint64_t tableBits = extraBits(perm, mt, level, false);
        
        const size_t idx = getIndex(vaddr, level);
        const Decoded decoded = decode(currentTable[idx], !level);

        if (decoded == Decoded_Mapping) {
            kpanic("Overlapping mapping at 0x%016zX.\n"
                "PTE is 0x%016zX.", vaddr, currentTable[idx]);
        } else if (decoded == Decoded_Table) {
            // Iterate to the next level
        } else if (decoded == Decoded_Empty) {
            if (canMap(size, vaddr, paddr, currentStepSize)) {
                makeMappingAt(currentTable + idx, paddr, mapBits);
                return currentStepSize;
            }
        }

        if (level == 0)
            kpanic("Bottom level reached. Mapping not possible.\n"
                "PTE is 0x%016zX.", (size_t)decoded);

        currentTable = makeTableAt(&currentTable[idx], tableBits);
        --level;
        currentStepSize = bit(entryMaskBits(level));
    }
}


static uint64_t extraBits(Perms perm, __attribute__((unused)) MemoryType mt, uint64_t level, bool mapping) {
    // Present bit enabled
    uint64_t bits = bit(0);
    
    // Supervisor bit
    if (level != 0)
        bits |= bit(2);

    // Level 0 table doesn't need bit 7 set
    if (mapping && level != 0)
        bits |= bit(7);

    // Write bit
    if (perm & Perm_W)
        bits |= bit(1);

    // Disable execute bit
    if (~perm & Perm_X)
        bits |= bit(63);

    return bits;
}


inline static size_t entryMaskBits(uint64_t level) {
    // Base index uses 12 bits.
    // Page directories use one 9-bit index for each level.
    return 12 + (level * 9);
}


inline static size_t getIndex(Vaddr vaddr, uint64_t level) {
    return (vaddr >> entryMaskBits(level)) & firstBits(9);
}


inline static size_t vaddrBaseOffset(Vaddr vaddr, uint64_t level) {
    return vaddr & firstBits(entryMaskBits(level));
}


inline Vaddr toVaddr(Paddr paddr) {
    return paddr + higherBase;
}


Paddr toPaddr(Vaddr vaddr) {
    Table table = (Table)toVaddr(getRootTable());

    // Walk page tables and get physical address
    for (int level = levels - 1; level >= 0; level--) {
        Entry nextEntry = table[getIndex(vaddr, level)];
        if (decode(nextEntry, !level) == Decoded_Mapping)
            return entryToAddress(nextEntry) + vaddrBaseOffset(vaddr, level);

        // Get next table from current entry
        table = (Table)toVaddr(entryToAddress(nextEntry));
    }

    kpanic("Could not get paddr of vaddr: 0x%016zX", vaddr);
}


Decoded decode(Entry entry, bool bottomLevel) {
    // Present bit is off, then entry is empty
    if ((entry & bit(0)) == 0)
        return Decoded_Empty;
    
    // Entry contains the paddr of a page frame
    if (bottomLevel || entry & bit(7))
        return Decoded_Mapping;

    // Entry contains an address to the next table
    return Decoded_Table;
}


static bool canMap(size_t size, Vaddr vaddr, Paddr paddr, size_t pageSize) {
    // We can only map if page size is below 1GiB and less than the amount of memory to map
    if (pageSize > GiB(1) || pageSize > size)
        return false;
    
    // We can only map if both addresses are page-size-aligned 
    const uint64_t mask = pageSize - 1;
    if (vaddr & mask) return false;
    if (paddr & mask) return false;
    
    return true;
}


static Paddr entryToAddress(Entry entry) {
    return entry & ADDR_MASK;
}


static Table makeTableAt(Entry *entry, uint64_t bits) {
    Decoded decoded = decode(*entry, false);

    if (decoded == Decoded_Mapping)
        // We already checked and called kpanic()
        __builtin_unreachable();
    else if (decoded == Decoded_Table)
        // Table is already here, just return it
        return (Table)entryToAddress(*entry);

    // There's no table here, allocate one and fill entry
    Table ret = (Table)kallocFrame();
    *entry = toPaddr((Vaddr)ret) | bits;
    return ret;
}


inline static void makeMappingAt(Entry *entry, Paddr paddr, uint64_t bits) {
    *entry = (paddr & ADDR_MASK) | bits;
}