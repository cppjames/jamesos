#include <kernel/kinfo.h>
#include <stdio.h>
#include <vga.h>

void print_splash_info() {
    const enum vga_color color1 = VGA_COLOR_WHITE;
    const enum vga_color color2 = VGA_COLOR_LIGHT_GREEN;

    setcolor(color1);
    printf("%s", "\nWelcome to...\n");

    char* splash =
    "&     _                            &___  ____  \n"
    "&    | | __ _ _ __ ___   ___  ___ &/ _ \\/ ___| \n"
    "& _  | |/ _` | '_ ` _ \\ / _ \\/ __|& | | \\___ \\ \n"
    "&| |_| | (_| | | | | | |  __/\\__ \\& |_| |___) |\n"
    "& \\___/ \\__,_|_| |_| |_|\\___||___/&\\___/|____/ \n\n";

    enum vga_color color = color1;
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

    setcolor(VGA_COLOR_LIGHT_GREY);
}