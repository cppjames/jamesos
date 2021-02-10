#include <kernel/stivale2.h>
#include <kernel/kinfo.h>
#include <stdio.h>

void print_splash_info(struct stivale2_struct *info) {
    const enum vga_color color0 = VGA_COLOR_DARK_GREY;
    const enum vga_color color1 = VGA_COLOR_WHITE;
    const enum vga_color color2 = VGA_COLOR_LIGHT_GREEN;
    const enum vga_color color3 = VGA_COLOR_LIGHT_CYAN;

    setcolor(color1);
    printf("%s", "\nWelcome to...\n");

    char* splash =
"::::::'##::::'###::::'##::::'##:'########::'######:::'&#######:::'######::\n"
":::::: ##:::'## ##::: ###::'###: ##.....::'##... ##:'&##.... ##:'##... ##:\n"
":::::: ##::'##:. ##:: ####'####: ##::::::: ##:::..:: &##:::: ##: ##:::..::\n"
":::::: ##:'##:::. ##: ## ### ##: ######:::. ######:: &##:::: ##:. ######::\n"
"'##::: ##: #########: ##. #: ##: ##...:::::..... ##: &##:::: ##::..... ##:\n"
" ##::: ##: ##.... ##: ##:.:: ##: ##:::::::'##::: ##: &##:::: ##:'##::: ##:\n"
". ######:: ##:::: ##: ##:::: ##: ########:. ######::. &#######::. ######::\n"
":......:::..:::::..::..:::::..::........:::......::::.......::::......:::\n\n";

    enum vga_color color = color1;
    char flag = 0;
    for (char* crt = splash; *crt; crt++) {
        if (*crt == ':' || *crt == '.' || *crt == '\'')
            color = color0;
        else {
            if (*crt == '&') {
                flag = 1;
                continue;
            }
            if (*crt == '\n')
                flag = 0;
            
            color = flag ? color1 : color2;
        }

        setcolor(color);
        putchar(*crt);
    }

    struct stivale2_tag* node = (struct stivale2_tag*)info->tags;
    while (node) {
        if (node->identifier == STIVALE2_STRUCT_TAG_FIRMWARE_ID) {
            setcolor(VGA_COLOR_LIGHT_GREY);
            printf("%s", " Firmware: ");
            setcolor(color3);
            printf("%s\n", (((struct stivale2_struct_tag_firmware*)node)->flags & 1) ? "BIOS" : "UEFI");
        }

        node = (struct stivale2_tag*)(node->next);
    }

    setcolor(VGA_COLOR_LIGHT_GREY);

    printf(" Bootloader: ");
    setcolor(color3);
    printf("%s ", info->bootloader_brand);
    setcolor(color1);
    printf("%s\n\n", info->bootloader_version);

    setcolor(VGA_COLOR_DARK_GREY);
    printf(" - - - - - - - - - - - - - - - - - - - - - - - -\n\n");

    setcolor(VGA_COLOR_LIGHT_GREY);
}

void klog_info(enum klog_status status, char* format, ...) {
    setcolor(VGA_COLOR_WHITE);
    printf("[");

    if (status == KLOG_SUCCESS) {
        setcolor(VGA_COLOR_LIGHT_GREEN);
        printf("DONE");
    } else if (status == KLOG_INFO) {
        setcolor(VGA_COLOR_LIGHT_CYAN);
        printf("INFO");
    } else if (status == KLOG_WARN) {
        setcolor(VGA_COLOR_LIGHT_BROWN);
        printf("WARNING");
    } else if (status == KLOG_FAIL) {
        setcolor(VGA_COLOR_LIGHT_RED);
        printf("FAIL");
    } else if (status == KLOG_PANIC) {
        setcolor(VGA_COLOR_LIGHT_MAGENTA);
        printf("PANIC");
    }

    setcolor(VGA_COLOR_WHITE);
    printf("] ");

    setcolor(VGA_COLOR_LIGHT_GREY);

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    putchar('\n');
    va_end(va);
}