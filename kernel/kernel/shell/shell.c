#include <kernel/shell.h>
#include "shell_internal.h"

#include <devices/keyboard.h>
#include <devices/terminal.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

unsigned char command[COMMAND_SIZE] = { 0 };
size_t commandCursor = 0;
size_t outCursorX = 0;
size_t outCursorY = 0;


void initShell(void) {
    outCursorX = terminalCursorX();
    outCursorY = terminalCursorY();

    renderCommand();

    while (true) {
        KeyEvent ev;
        if (isKeyEvent()) {
            ev = getKeyEvent();
            if (!ev.key.press)
                continue;

            if (isAscii(ev.key))
                addCursorCharacter(keyEventToAscii(ev));
            
            switch (ev.key.code) {
            case KeyCode_Backspace:
                if (!cursorAtStart())
                    deleteCharacter(commandCursor - 1);
                break;
            case KeyCode_Delete:
                if (!cursorAtEnd())
                    deleteCharacter(commandCursor);
                break;
            case KeyCode_LeftArrow:
                moveCursorLeft(1);
                break;
            case KeyCode_RightArrow:
                moveCursorRight(1);
                break;
            case KeyCode_Return:
            default:
                break;
            }
        }
    }
}


void addCursorCharacter(unsigned char ch) {
    clearCommand();

    size_t len = strlen((char*)(command + commandCursor)) + 1;
    for (size_t current = 0; current < len; current++) {
        command[commandCursor + len - current] = command[commandCursor + len - current - 1];
    }

    command[commandCursor++] = ch;
    renderCommand();
}


void deleteCharacter(size_t index) {
    clearCommand();

    unsigned char *commandSubstring = command + index;
    memmove(commandSubstring, commandSubstring + 1, strlen((char*)commandSubstring) + 1);
    if (index < commandCursor)
        commandCursor--;

    renderCommand();
}


void moveCursorLeft(size_t amount) {
    clearCommand();
    commandCursor = (amount > commandCursor) ? 0 : (commandCursor - amount);
    renderCommand();
}


void moveCursorRight(size_t amount) {
    clearCommand();

    size_t len = strlen((char*)command);
    if (commandCursor + amount > len)
        commandCursor = len;
    else
        commandCursor += amount;

    renderCommand();
}


inline bool cursorAtStart(void) {
    return commandCursor == 0;
}


inline bool cursorAtEnd(void) {
    return command[commandCursor] == 0;
}


void clearCommand(void) {
    terminalMoveCursor(outCursorX, outCursorY);
    terminalSetColor(vgaColorEntry(VgaColor_LightGray, VgaColor_Black));

    for (size_t current = 0; command[current]; current++) {
        putchar(' ');
        if (command[current] == '\n')
            putchar('\n');
    }
    
    putchar(' ');
}


void renderCommand(void) {
    terminalMoveCursor(outCursorX, outCursorY);
    
    for (size_t current = 0; command[current]; current++) {
        if (current == commandCursor)
            terminalSetColor(COLOR_HIGHLIGHT);
        else
            terminalSetColor(COLOR_NORMAL);
        
        if (command[current] == '\n')
            putchar(' ');
        putchar(command[current]);
    }

    if (command[commandCursor] == '\0')
        terminalSetColor(COLOR_HIGHLIGHT);
    else
        terminalSetColor(COLOR_NORMAL);
        
    putchar(' ');
    terminalSetColor(COLOR_NORMAL);
}