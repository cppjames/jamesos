#include <kernel/stivale2.h>
#include <kernel/kinfo.h>
#include <stdio.h>

void print_splash_info(struct stivale2_struct *info) {
    const enum VgaColor color1 = VgaColor_White;
    const enum VgaColor color2 = VgaColor_LightGreen;
    const enum VgaColor color3 = VgaColor_LightCyan;

    setcolor(color1);
    printf("%s", "\nWelcome to...\n");

    char* splash =
    "&     _                            &___  ____  \n"
    "&    | | __ _ _ __ ___   ___  ___ &/ _ \\/ ___| \n"
    "& _  | |/ _` | '_ ` _ \\ / _ \\/ __|& | | \\___ \\ \n"
    "&| |_| | (_| | | | | | |  __/\\__ \\& |_| |___) |\n"
    "& \\___/ \\__,_|_| |_| |_|\\___||___/&\\___/|____/ \n\n";

    enum VgaColor color = color1;
    for (char* crt = splash; *crt; crt++) {
        if (*crt == '&')
            if (color == color2)
                color = color1;
            else
                color = color2;
        else
            putchar(*crt);
        setcolor(color);
    }

    struct stivale2_tag* node = (struct stivale2_tag*)info->tags;
    while (node) {
        if (node->identifier == STIVALE2_STRUCT_TAG_FIRMWARE_ID) {
            setcolor(VgaColor_LightGray);
            printf("%s", " Firmware: ");
            setcolor(color3);
            printf("%s\n", (((struct stivale2_struct_tag_firmware*)node)->flags & 1) ? "BIOS" : "UEFI");
        }

        node = (struct stivale2_tag*)(node->next);
    }

    setcolor(VgaColor_LightGray);

    printf(" Bootloader: ");
    setcolor(color3);
    printf("%s ", info->bootloader_brand);
    setcolor(color1);
    printf("%s\n\n", info->bootloader_version);

    setcolor(VgaColor_DarkGray);
    printf(" - - - - - - - - - - - - - - - - - - - - - - - -\n\n");

    setcolor(VgaColor_LightGray);
}

void klog_info(enum klog_status status, char* format, ...) {
    setcolor(VgaColor_White);
    printf("[");

    if (status == KLOG_SUCCESS) {
        setcolor(VgaColor_LightGreen);
        printf("DONE");
    } else if (status == KLOG_INFO) {
        setcolor(VgaColor_LightCyan);
        printf("INFO");
    } else if (status == KLOG_WARN) {
        setcolor(VgaColor_LightBrown);
        printf("WARNING");
    } else if (status == KLOG_FAIL) {
        setcolor(VgaColor_LightRed);
        printf("FAIL");
    } else if (status == KLOG_PANIC) {
        setcolor(VgaColor_LightMagenta);
        printf("PANIC");
    }

    setcolor(VgaColor_White);
    printf("] ");

    setcolor(VgaColor_LightGray);

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    putchar('\n');
    va_end(va);
}