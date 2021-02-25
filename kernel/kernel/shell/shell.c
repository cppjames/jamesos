#include <kernel/shell.h>

#include "shell_internal.h"
#include <devices/keyboard.h>
#include <devices/terminal.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

unsigned char command[COMMAND_SIZE] = { 0 };
size_t command_cursor = { 0 };
size_t out_cursor_x = { 0 };
size_t out_cursor_y = { 0 };

#define COLOR_NORMAL    vga_entry_color(VgaColor_LightGray, VgaColor_Black)
#define COLOR_HIGHLIGHT vga_entry_color(VgaColor_Black, VgaColor_LightGray)

void init_shell() {
    out_cursor_x = terminal_cursor_x();
    out_cursor_y = terminal_cursor_y();

    renderCommand();

    while (true) {
        KeyEvent ev;
        if (isKeyEvent()) {
            ev = getKeyEvent();
            if (!ev.key.press)
                continue;

            if (isAscii(ev.key)) {
                addCursorCharacter(keyEventToAscii(ev));
            }
            
            switch (ev.key.code) {
            case KeyCode_Backspace:
                if (!cursorAtStart())
                    deleteCharacter(command_cursor - 1);
                break;
            case KeyCode_Delete:
                if (!cursorAtEnd())
                    deleteCharacter(command_cursor);
                break;
            case KeyCode_LeftArrow:
                moveCursorLeft(1);
                break;
            case KeyCode_RightArrow:
                moveCursorRight(1);
                break;
            case KeyCode_Return:
                break;
            default:
                break;
            }
        }
    }
}

void addCursorCharacter(unsigned char ch) {
    clearCommand();
    size_t len = strlen((char*)(command + command_cursor)) + 1;
    for (size_t current = 0; current < len; current++) {
        command[command_cursor + len - current] = command[command_cursor + len - current - 1];
    }
    command[command_cursor++] = ch;
    renderCommand();
}

void deleteCharacter(size_t index) {
    clearCommand();

    unsigned char *command_substring = command + index;
    memmove(command_substring, command_substring + 1, strlen((char*)command_substring) + 1);
    if (index < command_cursor)
        command_cursor--;

    renderCommand();
}

void moveCursorLeft(size_t amount) {
    clearCommand();
    command_cursor = (amount > command_cursor) ? 0 : (command_cursor - amount);
    renderCommand();
}

void moveCursorRight(size_t amount) {
    clearCommand();

    size_t len = strlen((char*)command);
    if (command_cursor + amount > len)
        command_cursor = len;
    else
        command_cursor += amount;

    renderCommand();
}

inline bool cursorAtStart() {
    return command_cursor == 0;
}

inline bool cursorAtEnd() {
    return command[command_cursor] == 0;
}

void clearCommand() {
    terminal_mvcursor(out_cursor_x, out_cursor_y);
    terminal_setcolor(vga_entry_color(VgaColor_LightGray, VgaColor_Black));

    for (size_t current = 0; command[current]; current++) {
        putchar(' ');
        if (command[current] == '\n')
            putchar('\n');
    }
    putchar(' ');
}

void renderCommand() {
    terminal_mvcursor(out_cursor_x, out_cursor_y);
    
    for (size_t current = 0; command[current]; current++) {
        if (current == command_cursor)
            terminal_setcolor(COLOR_HIGHLIGHT);
        else
            terminal_setcolor(COLOR_NORMAL);
        
        if (command[current] == '\n')
            putchar(' ');
        putchar(command[current]);
    }

    if (command[command_cursor] == '\0')
        terminal_setcolor(COLOR_HIGHLIGHT);
    else
        terminal_setcolor(COLOR_NORMAL);
        
    putchar(' ');

    terminal_setcolor(COLOR_NORMAL);
}