#define MODULE pmm

#include <kernel/system/pmm.h>
#include <kernel/system/vmm.h>

#include <kernel/kcontrol.h>
#include <kernel/kdebug.h>
#include <kernel/kinfo.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define FREE 0
#define USED 1

// Size of the buffer that will be used to hold allocated frames.
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

static void initMemoryMap(void);
static void initBitmap(void);

uint8_t *pageBitmap = 0;
MMap memoryMap = { 0 };

Paddr highestAddress = 0;
size_t maxPages = 0;


void initPmm(void) {
    initMemoryMap();
    initBitmap();

    kinfoLog(Log_Success, "PMM initialized.");
}


static void *getFrame(size_t index) {
    return (void*)(index * PAGE_SIZE);
}


static size_t getFrameIndex(void *frame) {
    return (size_t)frame / PAGE_SIZE;
}


static bool getFrameStatus(size_t index) {
    return pageBitmap[index / 8] & (1 << (index % 8));
}


static void setFrameStatus(size_t index, bool status) {
    size_t bitmask = 1 << (index % 8);

    if (status)
        pageBitmap[index / 8] |= bitmask;
    else
        pageBitmap[index / 8] &= ~bitmask;
}


static void *kallocFrameFind(void)
{
    // Find the first free page frame in bitmap
    for (size_t i = 0; i < maxPages; i++) {
        if (getFrameStatus(i) == FREE) {
            setFrameStatus(i, USED);
            return getFrame(i);
        }
    }

    return 0;
}


void *kallocFrame(void) {
    static void *frameBuffer[FRAME_BUFFER_SIZE] = { 0 };
    static size_t frameIndex = 0;

    // Buffer is empty, allocate again
    if (frameIndex == 0)
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
            frameBuffer[i] = kallocFrameFind();

    // Clear the page frame
    void *frame = frameBuffer[frameIndex];
    memset(frame, 0, PAGE_SIZE);

    frameIndex = (frameIndex + 1) % FRAME_BUFFER_SIZE;
    return frame;
}


void kfreeFrame(void *frame) {
    setFrameStatus(getFrameIndex(frame), FREE);
}


static void initMemoryMap(void) {
    StivaleTagMemmap *const memmapTag = kinfoGetMemmapTag();
    memoryMap.count = memmapTag->entries;
    const size_t mmapSize = sizeof(StivaleMMapEntry) * memmapTag->entries;
    const size_t alignedSize = alignUp(mmapSize, PAGE_SIZE);

    // Find some space to store the memory map data structure
    for (size_t i = 0; i < memmapTag->entries; i++) {
        StivaleMMapEntry *entry = &memmapTag->memmap[i];

        // Found large enough entry, store memory map here
        if (entry->type == MMType_Usable && entry->length > alignedSize) {
            memoryMap.entries = (MMEntry*)toVaddr(entry->base);
            entry->base += alignedSize;
            entry->length -= alignedSize;
            break;
        }
    }

    if (!memoryMap.entries)
        kpanic("Memory not sufficient to store memory map structure.");

    for (size_t i = 0; i < memmapTag->entries; i++) {
        StivaleMMapEntry entry = memmapTag->memmap[i];

        memoryMap.entries[i] = (MMEntry) {
            .base   = entry.base,
            .length = entry.length,
            .type   = (MMType)entry.type
        };
    }

    kinfoLog(Log_Info, "Initialized memory map at 0x%016zX", (uint64_t)memoryMap.entries);
}


static void initBitmap(void) {
    // Find the largest usable memory address
    for (size_t i = 0; i < memoryMap.count; i++) {
        MMEntry *entry = &memoryMap.entries[i];
        if (entry->type != MMType_Usable)
            continue;

        const Paddr top = entry->base + entry->length;
        if (top > highestAddress)
            highestAddress = top;
    }

    // Calculate bitmap size
    maxPages = highestAddress / PAGE_SIZE;
    const size_t bitmapSize = maxPages / 8;
    const size_t alignedSize = alignUp(bitmapSize, PAGE_SIZE);

    kinfoLog(Log_Info, "Bitmap size: %zuK", alignedSize / 1024);

    // Find some space to store the bitmap
    for (size_t i = 0; i < memoryMap.count; i++) {
        MMEntry *entry = &memoryMap.entries[i];

        // Found large enough entry, store bitmap here
        if (entry->type == MMType_Usable && entry->length > alignedSize) {
            pageBitmap = (uint8_t*)toVaddr(entry->base);
            entry->base += alignedSize;
            entry->length -= alignedSize;
            break;
        }
    }

    if (!pageBitmap)
        kpanic("Memory not sufficient to store page bitmap structure.");

    for (size_t page = 0; page < maxPages; page++)
        setFrameStatus(page, USED);

    // Mark usable memory in bitmap
    for (size_t i = 0; i < memoryMap.count; i++) {
        MMEntry *entry = &memoryMap.entries[i];

        if (entry->type != MMType_Usable)
            continue;

        // Calculate number of pages to set usable in bitmap
        Paddr startPaddr = alignUp(entry->base, PAGE_SIZE);
        size_t startPage = startPaddr / PAGE_SIZE;
        size_t sizeToSet = entry->length - (startPaddr - entry->base);
        size_t pagesToSet = alignDown(sizeToSet, PAGE_SIZE) / PAGE_SIZE;

        // Set pages as usable
        for (size_t page = 0; page < pagesToSet; page++)
            setFrameStatus(startPage + page, FREE);
    }

    kinfoLog(Log_Info, "Loaded bitmap at 0x%016zX", (uint64_t)pageBitmap);
}