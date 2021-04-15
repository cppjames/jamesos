#include <kernel/system/memory.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>
#include <kernel/kcontrol.h>

#define PAGE_SIZE   4096UL
#define ENTRY_COUNT 512UL
#define ADDR_MASK   0xFFFFFFFFFFFFF000

static uint64_t extraBits(Perms perm, MemoryType mt, uint64_t level, bool mapping);
static size_t getIndex(uint64_t vaddr, uint64_t level);
static size_t vaddrBaseOffset(uint64_t vaddr, uint64_t level);
static uint64_t toVaddr(uint64_t paddr);
static uint64_t toPaddr(uint64_t vaddr);
static bool canMap(uint64_t size, uint64_t vaddr, uint64_t paddr, uint64_t large_step);
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
    size += PAGE_SIZE - 1;
    size &= ~(PAGE_SIZE - 1);

    while (size != 0) {
        uint64_t level = levels - 1;
    
        uint64_t current_step_size = PAGE_SIZE << (9 * level);
        uint64_t *current_table = root_table;

        while (true) {
            const size_t idx = getIndex(vaddr, level);
            
            Decoded decoded = decode(current_table[idx], level == 0);

            // We can only map at this level if it's not a table
            if (decoded == Decoded_Mapping) {
                kpanic("Overlapping mapping at 0x%016zX.\n"
                    "PTE is 0x%016zX.", vaddr, current_table[idx]);
            } else if (decoded == Decoded_Table) {
                // Iterate to the next level
            } else if (decoded == Decoded_Empty) {
                if (canMap(size, vaddr, paddr, current_step_size)) {
                    const uint64_t map_bits = extraBits(perm, mt, level, true);
                    makeMappingAt(&current_table[idx], paddr, map_bits);
                    break;
                }
            }

            if (level == 0)
                kpanic("Bottom level reached. Mapping not possible.\n"
                    "PTE is 0x%016zX.", (size_t)decoded);

            const uint64_t tbl_bits = extraBits(perm, mt, level, false);
            current_table = makeTableAt(&current_table[idx], tbl_bits);
            current_step_size >>= 9;
            level--;
        }

        vaddr += current_step_size;
        paddr += current_step_size;
        size -= current_step_size;
    }
}


static uint64_t extraBits(Perms perm, __attribute__((unused)) MemoryType mt, uint64_t level, bool mapping) {

    // Present bit enabled
    uint64_t bits = (1UL << 0);
    
    // Supervisor bit
    if (level != 0)
        bits |= (1UL << 2);

    // Level 0 table doesn't need bit 7 set
    if (mapping && level != 0)
        bits |= (1UL << 7);

    // Write bit
    if (perm & Perm_W)
        bits |= (1UL << 1);

    // Disable execute bit
    if (~perm & Perm_X)
        bits |= (1UL << 63);

    return bits;
}


static size_t getIndex(uint64_t vaddr, uint64_t level) {
    const uint8_t shift_bits = 12 + 9 * level;
    const uint64_t mask = (1 << 9) - 1;
    return (vaddr >> shift_bits) & mask;
}


static size_t vaddrBaseOffset(uint64_t vaddr, uint64_t level) {
    uint64_t mask = { (PAGE_SIZE << (9 * level)) - 1 };
    return vaddr & mask;
}


static uint64_t toVaddr(uint64_t paddr) {
    return paddr + higher_base;
}


static uint64_t toPaddr(uint64_t vaddr) {
    uint64_t *table = (uint64_t*)toVaddr(getTable());

    // Walk page tables and get physical address
    for (int level = levels - 1; level >= 0; level--) {
        uint64_t next_entry = table[getIndex(vaddr, level)];
        if (decode(next_entry, level == 0) == Decoded_Mapping)
            return entryToAddress(next_entry) + vaddrBaseOffset(vaddr, level);

        table = (uint64_t*)toVaddr(entryToAddress(next_entry));
    }

    __builtin_unreachable();
}


Decoded decode(uint64_t entry, bool bottom_level) {
    if ((entry & 1) == 0)
        return Decoded_Empty;
    if (bottom_level || (entry & (1 << 7)))
        return Decoded_Mapping;
    return Decoded_Table;
}


static bool canMap(uint64_t size, uint64_t vaddr, uint64_t paddr, uint64_t large_step) {
    if (large_step > 0x40000000 || size < large_step)
        return false;
    
    const uint64_t mask = large_step - 1;
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
        return (uint64_t*)entryToAddress(*entry);

    uint64_t ret = (uint64_t)kallocFrame();
    *entry = toPaddr(ret) | bits;
    return (uint64_t*)ret;
}


inline static void makeMappingAt(uint64_t *entry, uint64_t paddr, uint64_t bits) {
    *entry = (paddr & ADDR_MASK) | bits;
}