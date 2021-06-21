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

    kinfoLog(Log_Success, "PMM initialized.");
}

static inline void *getFrame(size_t index) {
    return (void*)(index * PAGE_SIZE);
}

static inline size_t getFrameIndex(void *frame) {
    return (size_t)frame / PAGE_SIZE;
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
    // Find the first free page frame in bitmap
    for (size_t i = 0; i < max_pages; i++) {
        if (getFrameStatus(i) == FREE) {
            setFrameStatus(i, USED);
            return getFrame(i);
        }
    }

    return 0;
}

void *kallocFrame() {
    static void *frame_buffer[FRAME_BUFFER_SIZE] = { 0 };
    static size_t frame_index = 0;

    // Buffer is empty, allocate again
    if (frame_index == 0)
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
            frame_buffer[i] = kallocFrameFind();

    void *frame = frame_buffer[frame_index];
    memset(frame, 0, PAGE_SIZE);

    frame_index = (frame_index + 1) % FRAME_BUFFER_SIZE;
    return frame;
}

void kfreeFrame(void *frame) {
    setFrameStatus(getFrameIndex(frame), FREE);
}

static void initMemoryMap() {
    StivaleTagMemmap *const memmap_tag = kinfoGetMemmapTag();
    memory_map.count = memmap_tag->entries;
    const size_t mmap_size = sizeof(StivaleMMapEntry) * memmap_tag->entries;
    const size_t aligned_size = alignUp(mmap_size, PAGE_SIZE);

    // Find some space to store the memory map data structure
    for (size_t i = 0; i < memmap_tag->entries; i++) {
        StivaleMMapEntry *entry = &memmap_tag->memmap[i];

        // Found large enough entry, store memory map here
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
        StivaleMMapEntry entry = memmap_tag->memmap[i];

        memory_map.entries[i] = (MMEntry) {
            .base   = entry.base,
            .length = entry.length,
            .type   = (MMType)entry.type
        };
    }

    kinfoLog(Log_Info, "Initialized memory map at 0x%016zX", (uint64_t)memory_map.entries);
}

static void initBitmap() {
    // Find the largest usable memory address
    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];
        if (entry->type != MMType_Usable)
            continue;

        const Paddr top = entry->base + entry->length;
        if (top > highest_address)
            highest_address = top;
    }

    // Calculate bitmap size
    max_pages = highest_address / PAGE_SIZE;
    const size_t bitmap_size = max_pages / 8;
    const size_t aligned_size = alignUp(bitmap_size, PAGE_SIZE);

    kinfoLog(Log_Info, "Bitmap size: %zuK", aligned_size / 1024);

    // Find some space to store the bitmap
    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];

        // Found large enough entry, store bitmap here
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

    // Mark usable memory in bitmap
    for (size_t i = 0; i < memory_map.count; i++) {
        MMEntry *entry = &memory_map.entries[i];

        if (entry->type != MMType_Usable)
            continue;

        // Calculate number of pages to set usable in bitmap
        Paddr start_paddr = alignUp(entry->base, PAGE_SIZE);
        size_t start_page = start_paddr / PAGE_SIZE;
        size_t size_to_set = entry->length - (start_paddr - entry->base);
        size_t pages_to_set = alignDown(size_to_set, PAGE_SIZE) / PAGE_SIZE;

        for (size_t page = 0; page < pages_to_set; page++)
            setFrameStatus(start_page + page, FREE);
    }

    kinfoLog(Log_Info, "Loaded bitmap at 0x%016zX", (uint64_t)page_bitmap);
}