#include <kernel/system/memory.h>

#include <stdint.h>
#include <stddef.h>

#include <kernel/kdebug.h>

#define PAGE_SIZE 4096
#define ENTRY_COUNT 512

extern void enablePAE();
static void loadPageDirectory(uint64_t *page_dir);
extern void enablePaging();

uint64_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));

uint64_t *page_dir = { 0 };
uint64_t *first_page_table = { 0 };

void init_paging() {
    page_dir = (uint64_t*)kallocFrame();
    for (size_t i = 0; i < ENTRY_COUNT; i++)
        page_dir[i] = 0x00000002;

    first_page_table = (uint64_t*)kallocFrame();
    for (size_t i = 0; i < ENTRY_COUNT; i++)
        first_page_table[i] = (i * 0x1000) | 0b11;

    page_dir[0] = ((uint64_t)first_page_table) | 0b11;
    page_dir_ptr_tab[0] = (uint64_t)page_dir | 1;

    klog_debug("%p\n", (void*)page_dir_ptr_tab);

    enablePAE();
    loadPageDirectory(page_dir_ptr_tab);
    enablePaging();
}

void loadPageDirectory(uint64_t *page_dir) {
    asm volatile ("mov cr3, %0" :: "r" (page_dir));
}

/*********************************
 *   WARNING! WORK IN PROGRESS   *
 *********************************/

void map(uint64_t vaddr, uint64_t paddr, uint64_t size, Perms perm, MemoryType mt) {
    
    size += PAGE_SIZE - 1;
    size &= ~(PAGE_SIZE - 1);

    //var root: table_ptr = cr3;

    //const levels = 4

    //const base_bits = @intCast(u6, base_bits);

    //const small_bits = extra_bits(perm, mt, page_size, true);
    //const large_bits = extra_bits(perm, mt, page_size, false);

    //while(size != 0) {
    //    var current_step_size = page_size << @intCast(u6, (base_bits - 3) * (levels - 1));
    //    var level = levels - 1;
    //    var current_table = root;

    //    while(true) {
    //        const ind = get_index(vaddr, base_bits, level);
    //        // We can only map at this level if it's not a table
    //        switch(decode(current_table[ind], level == 0)) {
    //            .Mapping => {
    //                sabaton.log_hex("Overlapping mapping at ", vaddr);
    //                sabaton.log_hex("PTE is ", current_table[ind]);
    //                unreachable;
    //            },
    //            .Table => { }, // Just iterate to the next level
    //            .Empty => {
    //                // If we can map at this level, do so
    //                if(can_map(size, vaddr, paddr, current_step_size)) {
    //                    const bits = if(level == 0) small_bits else large_bits;
    //                    make_mapping_at(&current_table[ind], paddr, bits);
    //                    //sabaton.log_hex("At idx ", ind);
    //                    break;
    //                }
    //                // Otherwise, just iterate to the next level
    //            }
    //        }

    //        if(level == 0)
    //            unreachable;

    //        current_table = make_table_at(&current_table[ind]);
    //        current_step_size >>= (base_bits - 3);
    //        level -= 1;
    //    }

    //    vaddr += current_step_size;
    //    paddr += current_step_size;
    //    size -= current_step_size;
    //}
}