#include <kernel/system/memory.h>

#include <stdint.h>
#include <stddef.h>

#include <kernel/kdebug.h>

#define ENTRY_COUNT 512

extern void enablePAE();
void loadPageDirectory(uint64_t *page_dir);
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

    // Paging tutorial said "put first page table in the directory"...
    //page_dir[0] = ((uint64_t)first_page_table) | 0b11;

    page_dir[0] = 0b10000011; // ...but PAE paging tutorial said do this?!
    page_dir_ptr_tab[0] = (uint64_t)&page_dir | 1;

    klog_debug("%p\n", (void*)page_dir_ptr_tab);

    enablePAE();
    loadPageDirectory(page_dir_ptr_tab);
    enablePaging();
}

void loadPageDirectory(uint64_t *page_dir) {
    asm volatile ("mov cr3, %0" :: "r" (page_dir));
}