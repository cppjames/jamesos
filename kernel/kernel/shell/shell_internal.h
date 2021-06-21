#ifndef SHELL_INTERNAL_H
#define SHELL_INTERNAL_H

#include <stddef.h>
#include <stdbool.h>

#define COLOR_NORMAL    vgaEntryColor(VgaColor_LightGray, VgaColor_Black)
#define COLOR_HIGHLIGHT vgaEntryColor(VgaColor_Black, VgaColor_LightGray)

#define COMMAND_SIZE 4096

void addCursorCharacter(unsigned char ch);
void deleteCharacter(size_t index);

void moveCursorLeft(size_t amount);
void moveCursorRight(size_t amount);
bool cursorAtStart();
bool cursorAtEnd();

void clearCommand();
void renderCommand();

#endif // SHELL_INTERNAL_H
