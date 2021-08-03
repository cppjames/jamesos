#include <kernel/kinfo.h>
#include <kernel/stivale2.h>

#include <devices/terminal.h>
#include <kernel/kdebug.h>

#include <stdio.h>
#include <string.h>

static VgaColor getStatusColor(LogStatus status);
static const char *getStatusText(LogStatus status);

StivaleTagMemmap  *memmap_tag  = 0;
StivaleTagCmdline *cmdline_tag = 0;


inline StivaleTagMemmap *kinfoGetMemmapTag(void) {
    return memmap_tag;
}


inline StivaleTagCmdline *kinfoGetCmdlineTag(void) {
    return cmdline_tag;
}


void kinfoParseStivaleStruct(StivaleStruct *info) {
    StivaleTag *node = (StivaleTag*)info->tags;
    while (node) {
        if (node->identifier == STIVALE2_STRUCT_TAG_MEMMAP_ID)
            memmap_tag = (typeof(memmap_tag))node;
        else if (node->identifier == STIVALE2_STRUCT_TAG_CMDLINE_ID)
            cmdline_tag = (typeof(cmdline_tag))node;

        node = (StivaleTag*)(node->next);
    }
}


void kinfoPrintSplash(StivaleStruct *info) {
    const VgaColor color1 = VgaColor_White;
    const VgaColor color2 = VgaColor_LightGreen;
    const VgaColor color3 = VgaColor_LightCyan;

    setcolor(color1);
    puts("\nWelcome to...");

    static const char *splash =
    "&     _                            &___  ____  \n"
    "&    | | __ _ _ __ ___   ___  ___ &/ _ \\/ ___| \n"
    "& _  | |/ _` | '_ ` _ \\ / _ \\/ __|& | | \\___ \\ \n"
    "&| |_| | (_| | | | | | |  __/\\__ \\& |_| |___) |\n"
    "& \\___/ \\__,_|_| |_| |_|\\___||___/&\\___/|____/ \n\n";

    VgaColor color = color1;

    // Print title in alternating colors
    for (const char *crt = splash; *crt; crt++) {
        if (*crt == '&') {
            color = (color == color2) ? color1 : color2;
            setcolor(color);
            continue;
        }
        
        putchar(*crt);
    }

    StivaleTag* node = (StivaleTag*)info->tags;

    // Find firmware tag and print its data
    while (node) {
        if (node->identifier == STIVALE2_STRUCT_TAG_FIRMWARE_ID) {
            setcolor(VgaColor_LightGray);
            printf("%s", " Firmware: ");
            setcolor(color3);
            printf("%s\n", (((StivaleTagFirmware*)node)->flags & 1) ? "BIOS" : "UEFI");
            break;
        }

        node = (StivaleTag*)(node->next);
    }

    setcolor(VgaColor_LightGray);
    printf(" Bootloader: ");
    setcolor(color3);
    printf("%s ", info->bootloader_brand);
    setcolor(color1);
    printf("%s\n", info->bootloader_version);

    // Print command line arguments, if there are any
    if (strlen((char*)kinfoGetCmdlineTag()->cmdline)) {
        setcolor(VgaColor_LightGray);
        printf(" Command line arguments: ");
        setcolor(color1);
        printf("%s\n", kinfoGetCmdlineTag()->cmdline);
    }

    setcolor(VgaColor_DarkGray);
    printf("\n - - - - - - - - - - - - - - - - - - - - - - - -\n\n");

    setcolor(VgaColor_LightGray);
}


void kinfoLog(LogStatus status, const char *format, ...) {
    va_list va;
    va_start(va, format);

    auto status_color = getStatusColor(status);
    auto status_test = getStatusText(status);

    setcolor(status_color);
    printf("%s", status_test);
    
    setcolor(VgaColor_DarkGray);
    printf("%s", " - ");

    setcolor(VgaColor_LightGray);
    vprintf(format, va);
    putchar('\n');

    va_end(va);
}


void kinfoModuleLog(const char *module, LogStatus status, const char *format, ...) {
    va_list va;
    va_start(va, format);

    auto status_color = getStatusColor(status);
    auto status_test = getStatusText(status);

    setcolor(status_color);
    printf("%s", status_test);
    
    setcolor(VgaColor_DarkGray);
    printf("%s", " :: ");

    setcolor(VgaColor_White);
    printf("%s", module);
    
    setcolor(VgaColor_DarkGray);
    printf("%s", " - ");

    setcolor(VgaColor_LightGray);
    vprintf(format, va);
    putchar('\n');

    va_end(va);
}


static VgaColor getStatusColor(LogStatus status) {
    static const VgaColor status_colors[] = {
        [Log_Success] = VgaColor_LightGreen,
        [Log_Info] = VgaColor_LightCyan,
        [Log_Warn] = VgaColor_LightBrown,
        [Log_Fail] = VgaColor_LightRed,
        [Log_Panic] = VgaColor_LightMagenta
    };

    return status_colors[status];
}


static const char *getStatusText(LogStatus status) {
    static const char *status_strings[] = {
        [Log_Success] = "DONE",
        [Log_Info] = "INFO",
        [Log_Warn] = "WARN",
        [Log_Fail] = "FAIL",
        [Log_Panic] = "PANIC"
    };

    return status_strings[status];
}