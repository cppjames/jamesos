#include <kernel/kinfo.h>
#include <kernel/stivale2.h>

#include <devices/terminal.h>
#include <kernel/kdebug.h>

#include <stdio.h>
#include <string.h>

static VgaColor getStatusColor(LogStatus status);
static const char *getStatusText(LogStatus status);

StivaleTagMemmap  *memmapTag  = 0;
StivaleTagCmdline *cmdlineTag = 0;


inline StivaleTagMemmap *kinfoGetMemmapTag(void) {
    return memmapTag;
}


inline StivaleTagCmdline *kinfoGetCmdlineTag(void) {
    return cmdlineTag;
}


void kinfoParseStivaleStruct(StivaleStruct *info) {
    StivaleTag *tagNode = (StivaleTag*)info->tags;
    while (tagNode) {
        if (tagNode->identifier == STIVALE2_STRUCT_TAG_MEMMAP_ID)
            memmapTag = (typeof(memmapTag))tagNode;
        else if (tagNode->identifier == STIVALE2_STRUCT_TAG_CMDLINE_ID)
            cmdlineTag = (typeof(cmdlineTag))tagNode;

        tagNode = (StivaleTag*)(tagNode->next);
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

    StivaleTag* tagNode = (StivaleTag*)info->tags;

    // Find firmware tag and print its data
    while (tagNode) {
        if (tagNode->identifier == STIVALE2_STRUCT_TAG_FIRMWARE_ID) {
            setcolor(VgaColor_LightGray);
            printf("%s", " Firmware: ");
            setcolor(color3);
            printf("%s\n", (((StivaleTagFirmware*)tagNode)->flags & 1) ? "BIOS" : "UEFI");
            break;
        }

        tagNode = (StivaleTag*)(tagNode->next);
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

    VgaColor statusColor = getStatusColor(status);
    const char* statusText = getStatusText(status);

    setcolor(statusColor);
    printf("%s", statusText);
    
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

    VgaColor statusColor = getStatusColor(status);
    const char* statusText = getStatusText(status);

    setcolor(statusColor);
    printf("%s", statusText);
    
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
    static const VgaColor statusColors[] = {
        [Log_Success] = VgaColor_LightGreen,
        [Log_Info] = VgaColor_LightCyan,
        [Log_Warn] = VgaColor_LightBrown,
        [Log_Fail] = VgaColor_LightRed,
        [Log_Panic] = VgaColor_LightMagenta
    };

    return statusColors[status];
}


static const char *getStatusText(LogStatus status) {
    static const char *statusStrings[] = {
        [Log_Success] = "DONE",
        [Log_Info] = "INFO",
        [Log_Warn] = "WARN",
        [Log_Fail] = "FAIL",
        [Log_Panic] = "PANIC"
    };

    return statusStrings[status];
}