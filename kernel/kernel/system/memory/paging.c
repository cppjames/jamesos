#include <kernel/system/memory.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>
#include <kernel/kcontrol.h>

#define ENTRY_COUNT 512UL
#define ADDR_MASK   ~firstBits(entryMaskBits(0))

static uint64_t mapLargest(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt);
static uint64_t extraBits(Perms perm, MemoryType mt, uint64_t level, bool mapping);
static size_t getIndex(uint64_t vaddr, uint64_t level);
static size_t entryMaskBits(uint64_t level);
static size_t vaddrBaseOffset(uint64_t vaddr, uint64_t level);
static uint64_t toVaddr(uint64_t paddr);
static uint64_t toPaddr(uint64_t vaddr);
static bool canMap(uint64_t size, uint64_t vaddr, uint64_t paddr, uint64_t page_size);
static uint64_t entryToAddress(uint64_t entry);
static uint64_t *makeTableAt(uint64_t *entry, uint64_t bits);
inline static void makeMappingAt(uint64_t *entry, uint64_t paddr, uint64_t bits);

extern void enableNXE();
extern void loadTable(uint64_t table);
extern uint64_t getTable();

uint64_t *root_table = { 0 };
const uint64_t levels = { 4 };

const uint64_t higher_base = { 0xFFFF800000000000 };
const uint64_t kernel_base = { 0xFFFFFFFF80000000 };


void init_paging() {
    root_table = (uint64_t*)kallocFrame();

    identity_map(0x0, GiB(5), Perm_RWX, MemoryType_Memory);
    map(higher_base, 0x0, GiB(5), Perm_RWX, MemoryType_Memory);
    map(kernel_base, 0x0, GiB(2), Perm_RWX, MemoryType_Memory);
    
    enableNXE();
    loadTable(toPaddr((uint64_t)root_table));

    klog_info(KLOG_SUCCESS, "Paging enabled.");
}


void map(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt) {
    size += firstBits(entryMaskBits(0));
    size &= ~firstBits(entryMaskBits(0));

    while (size) {
        uint64_t max_map_size = mapLargest(vaddr, paddr, size, perm, mt);

        vaddr += max_map_size;
        paddr += max_map_size;
        size -= max_map_size;
    }
}

static uint64_t mapLargest(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt) {
    uint64_t level = levels - 1;
    
    uint64_t current_step_size = bit(entryMaskBits(level));
    uint64_t *current_table = root_table;

    while (true) {
        const uint64_t map_bits = extraBits(perm, mt, level, true);
        const uint64_t tbl_bits = extraBits(perm, mt, level, false);
        
        const size_t idx = getIndex(vaddr, level);
        const Decoded decoded = decode(current_table[idx], !level);

        if (decoded == Decoded_Mapping) {
            kpanic("Overlapping mapping at 0x%016zX.\n"
                "PTE is 0x%016zX.", vaddr, current_table[idx]);
        } else if (decoded == Decoded_Table) {
            // Iterate to the next level
        } else if (decoded == Decoded_Empty) {
            if (canMap(size, vaddr, paddr, current_step_size)) {
                makeMappingAt(current_table + idx, paddr, map_bits);
                return current_step_size;
            }
        }

        if (level == 0)
            kpanic("Bottom level reached. Mapping not possible.\n"
                "PTE is 0x%016zX.", (size_t)decoded);

        current_table = makeTableAt(&current_table[idx], tbl_bits);
        --level;
        current_step_size = bit(entryMaskBits(level));
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
    return 12 + (level * 9);
}


inline static size_t getIndex(uint64_t vaddr, uint64_t level) {
    return (vaddr >> entryMaskBits(level)) & firstBits(9);
}


inline static size_t vaddrBaseOffset(uint64_t vaddr, uint64_t level) {
    return vaddr & firstBits(entryMaskBits(level));
}


inline static uint64_t toVaddr(uint64_t paddr) {
    return paddr + higher_base;
}


static uint64_t toPaddr(uint64_t vaddr) {
    uint64_t *table = (uint64_t*)toVaddr(getTable());

    // Walk page tables and get physical address
    for (int level = levels - 1; level >= 0; level--) {
        uint64_t next_entry = table[getIndex(vaddr, level)];
        if (decode(next_entry, !level) == Decoded_Mapping)
            return entryToAddress(next_entry) + vaddrBaseOffset(vaddr, level);

        // Get next table from current entry
        table = (uint64_t*)toVaddr(entryToAddress(next_entry));
    }

    kpanic("Could not get paddr of vaddr: 0x%016zX", vaddr);
}


Decoded decode(uint64_t entry, bool bottom_level) {
    // Present bit is off, then entry is empty
    if ((entry & bit(0)) == 0)
        return Decoded_Empty;
    
    // Entry contains the paddr of a page frame
    if (bottom_level || entry & bit(7))
        return Decoded_Mapping;

    // Entry contains an address to the next table
    return Decoded_Table;
}


static bool canMap(uint64_t size, uint64_t vaddr, uint64_t paddr, uint64_t page_size) {
    // We can only map if page size is below 1GiB and less than the amount of memory to map
    if (page_size > GiB(1) || page_size > size)
        return false;
    
    // We can only map if both addresses are page-size-aligned 
    const uint64_t mask = page_size - 1;
    if (vaddr & mask) return false;
    if (paddr & mask) return false;
    
    return true;
}


static uint64_t entryToAddress(uint64_t entry) {
    return entry & ADDR_MASK;
}


static uint64_t *makeTableAt(uint64_t *entry, uint64_t bits) {
    Decoded decoded = decode(*entry, false);

    if (decoded == Decoded_Mapping)
        // We already checked and called kpanic()
        __builtin_unreachable();
    else if (decoded == Decoded_Table)
        // Table is already here, just return it
        return (uint64_t*)entryToAddress(*entry);

    // There's no table here, allocate one and fill entry
    uint64_t ret = (uint64_t)kallocFrame();
    *entry = toPaddr(ret) | bits;
    return (uint64_t*)ret;
}


inline static void makeMappingAt(uint64_t *entry, uint64_t paddr, uint64_t bits) {
    *entry = (paddr & ADDR_MASK) | bits;
}