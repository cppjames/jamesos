#include <kernel/system/pmm.h>
#include <kernel/system/vmm.h>

#include <kernel/kinfo.h>
#include <kernel/kcontrol.h>
#include <kernel/kdebug.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define FREE 0
#define USED 1
#define PAGE_SIZE KiB(4)
#define FRAME_BUFFER_SIZE 16

typedef enum MMType {
    MMType_Usable                = 1,
    MMType_Reserved              = 2,
    MMType_AcpiReclaimable       = 3,
    MMType_AcpiNvs               = 4,
    MMType_BadMemory             = 5,
    MMType_BootloaderReclaimable = 0x1000,
    MMType_KernelAndModules      = 0x1001
} MMType;

typedef struct MMEntry {
    Paddr base;
    size_t length;
    MMType type;
} MMEntry;

typedef struct MMap {
    MMEntry *entries;
    size_t count;
} MMap;

static void initMemoryMap();
static void initBitmap();

uint8_t *page_bitmap = { 0 };
MMap memory_map = { 0 };
Paddr highest_address = { 0 };
size_t max_pages = { 0 };

void initPmm() {
    initMemoryMap();
    initBitmap();

    klog_info(KLOG_SUCCESS, "PMM initialized.");
}

static inline void *getStartFrame() {
    size_t kernel_end_address = 0xFFFF800000118000;
    return (void*)((kernel_end_address & ~(size_t)0x0FFF) + PAGE_SIZE);
}

static inline bool getFrameStatus(size_t index) {
    return page_bitmap[index / 8] & (1 << (index % 8));
}

static inline void setFrameStatus(size_t index, bool status) {
    size_t bitmask = 1 << (index % 8);

    if (status)
        page_bitmap[index / 8] |= bitmask;
    else
        page_bitmap[index / 8] &= ~bitmask;
}

static void *kallocFrameFind()
{
    for (size_t i = 0; i < max_pages; i++) {
        if (getFrameStatus(i) == FREE) {
            setFrameStatus(i, USED);
            return getStartFrame() + (i * PAGE_SIZE);
        }
    }

    return 0;
}

void *kallocFrame() {
    static void *frame_buffer[FRAME_BUFFER_SIZE] = { 0 };
    static size_t frame_index = 0;

    if (frame_index == 0)
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
            frame_buffer[i] = kallocFrameFind();

    void *frame = frame_buffer[frame_index];

    for (size_t i = 0; i < PAGE_SIZE; i++)
        ((unsigned char*)frame)[i] = 0x0;

    frame_index = (frame_index + 1) % FRAME_BUFFER_SIZE;
    return frame;
}

void kfreeFrame(void *frame) {
    void *frame_offset = (void*)(frame - getStartFrame());
    size_t index = (size_t)frame_offset / PAGE_SIZE;

    setFrameStatus(index, FREE);
}

static void initMemoryMap() {
    struct stivale2_struct_tag_memmap *memmap_tag = get_memmap_tag();
    memory_map.count = memmap_tag->entries;
    const size_t mmap_size = sizeof(struct stivale2_mmap_entry) * memmap_tag->entries;
    const size_t aligned_size = alignUp(mmap_size, PAGE_SIZE);

    for (size_t i = 0; i < memmap_tag->entries; i++) {
        struct stivale2_mmap_entry *entry = &memmap_tag->memmap[i];

        if (entry->type == MMType_Usable && entry->length > aligned_size) {
            memory_map.entries = (MMEntry*)toVaddr(entry->base);
            entry->base += aligned_size;
            entry->length -= aligned_size;
            break;
        }
    }

    if (!memory_map.entries)
        kpanic("Memory not sufficient to store memory map structure.");

    for (size_t i = 0; i < memmap_tag->entries; i++) {
        struct stivale2_mmap_entry entry = memmap_tag->memmap[i];

        memory_map.entries[i] = (MMEntry) {
            .base   = entry.base,
            .length = entry.length,
            .type   = (MMType)entry.type
        };
    }

    klog_info(KLOG_INFO, "Initialized memory map at 0x%016zX", (uint64_t)memory_map.entries);
}

static void initBitmap() {
    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];

        if (entry->type != MMType_Usable)
            continue;

        const Paddr top = entry->base + entry->length;
        if (top > highest_address)
            highest_address = top;
    }

    max_pages = highest_address / PAGE_SIZE;
    const size_t bitmap_size = max_pages / 8;
    const size_t aligned_size = alignUp(bitmap_size, PAGE_SIZE);

    klog_info(KLOG_INFO, "Bitmap size: %zuK", aligned_size / 1024);

    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];

        if (entry->type == MMType_Usable && entry->length > aligned_size) {
            page_bitmap = (uint8_t*)toVaddr(entry->base);
            entry->base += aligned_size;
            entry->length -= aligned_size;
            break;
        }
    }

    if (!page_bitmap)
        kpanic("Memory not sufficient to store page bitmap structure.");

    for (size_t page = 0; page < max_pages; page++)
        setFrameStatus(page, USED);

    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];

        if (entry->type != MMType_Usable)
            continue;

        Paddr start_paddr = alignUp(entry->base, PAGE_SIZE);
        size_t start_page = start_paddr / PAGE_SIZE;
        size_t size_to_set = entry->length - (start_paddr - entry->base);
        size_t pages_to_set = alignDown(size_to_set, PAGE_SIZE) / PAGE_SIZE;

        for (size_t page = 0; page < pages_to_set; page++)
            setFrameStatus(start_page + page, FREE);
    }

    klog_info(KLOG_INFO, "Loaded bitmap at 0x%016zX", (uint64_t)page_bitmap);
}