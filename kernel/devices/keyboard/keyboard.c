#include <devices/keyboard.h>
#include "keyboard_internal.h"

#include <stdio.h>
#include <kernel/kdebug.h>

keyboard_state_t keyboard_state = (keyboard_state_t) {
    .key = { 0 },
    .flags = 0x00
};

key_t codeToKey(unsigned char code) {
    return (key_t) {
        .code = (key_code_t)((code <= 128) ? code : (code - 128)),
        .press = code <= 128
    };
}

void sendKey(key_t key) {
    _updateStateMachine(key);

    if (key.press && isAscii(key)) {
        unsigned char ch = _getStateMachineUpper(key) ? keyToAsciiUpper(key) : keyToAscii(key);
        putchar(ch);
    }
}

void _updateStateMachine(key_t key) {
    _updateModifiers(key);
    _updateKeySwitches(key);
}

void _updateModifiers(key_t key) {
    key_flag_t flag = 0x0;

    if      (isShift(key)) flag = KeyFlag_Shift;
    else if (isCtrl(key))  flag = KeyFlag_Control;
    else if (isAlt(key))   flag = KeyCode_Alt;

    if (key.press)
        keyboard_state.flags |= flag;
    else
        keyboard_state.flags &= ~flag;
}

void _updateKeySwitches(key_t key) {
    if (!key.press) return;

    key_flag_t flag = 0x0;

    if (isCapsLock(key)) flag = KeyFlag_CapsLock;

    if (keyboard_state.flags & flag)
        keyboard_state.flags &= ~flag;
    else
        keyboard_state.flags |= flag;
}

bool _getStateMachineUpper(key_t key) {
    bool capslock = keyboard_state.flags & KeyFlag_CapsLock;
    bool shift = keyboard_state.flags & KeyFlag_Shift;
    bool upper = capslock && isLetter(key);
    return shift ? !upper : upper;
}

const char KEY_ASCII_MAP[256] = {
    [KeyCode_1] = '1',
    [KeyCode_2] = '2',
    [KeyCode_3] = '3',
    [KeyCode_4] = '4',
    [KeyCode_5] = '5',
    [KeyCode_6] = '6',
    [KeyCode_7] = '7',
    [KeyCode_8] = '8',
    [KeyCode_9] = '9',
    [KeyCode_0] = '0',

    [KeyCode_Line1_1] =  'q',
    [KeyCode_Line1_2] =  'w',
    [KeyCode_Line1_3] =  'e',
    [KeyCode_Line1_4] =  'r',
    [KeyCode_Line1_5] =  't',
    [KeyCode_Line1_6] =  'y',
    [KeyCode_Line1_7] =  'u',
    [KeyCode_Line1_8] =  'i',
    [KeyCode_Line1_9] =  'o',
    [KeyCode_Line1_10] = 'p',

    [KeyCode_Line2_1] =  'a',
    [KeyCode_Line2_2] =  's',
    [KeyCode_Line2_3] =  'd',
    [KeyCode_Line2_4] =  'f',
    [KeyCode_Line2_5] =  'g',
    [KeyCode_Line2_6] =  'h',
    [KeyCode_Line2_7] =  'j',
    [KeyCode_Line2_8] =  'k',
    [KeyCode_Line2_9] =  'l',

    [KeyCode_Line3_1] =  'z',
    [KeyCode_Line3_2] =  'x',
    [KeyCode_Line3_3] =  'c',
    [KeyCode_Line3_4] =  'v',
    [KeyCode_Line3_5] =  'b',
    [KeyCode_Line3_6] =  'n',
    [KeyCode_Line3_7] =  'm',

    [KeyCode_Backtick] = '`',
    [KeyCode_Minus] =  '-',
    [KeyCode_Equals] = '=',

    [KeyCode_BracketL] = '[',
    [KeyCode_BracketR] = ']',

    [KeyCode_Semicolon] = ';',
    [KeyCode_Quote] =     '\'',
    [KeyCode_Backslash] = '\\',

    [KeyCode_Comma] =  ',',
    [KeyCode_Period] = '.',
    [KeyCode_Slash] =  '/',

    [KeyCode_Return] = '\n',
    [KeyCode_Space] = ' '
};

const char KEY_ASCII_MAP_UPPER[256] = {
    [KeyCode_1] = '!',
    [KeyCode_2] = '@',
    [KeyCode_3] = '#',
    [KeyCode_4] = '$',
    [KeyCode_5] = '%',
    [KeyCode_6] = '^',
    [KeyCode_7] = '&',
    [KeyCode_8] = '*',
    [KeyCode_9] = '(',
    [KeyCode_0] = ')',

    [KeyCode_Line1_1] =  'Q',
    [KeyCode_Line1_2] =  'W',
    [KeyCode_Line1_3] =  'E',
    [KeyCode_Line1_4] =  'R',
    [KeyCode_Line1_5] =  'T',
    [KeyCode_Line1_6] =  'Y',
    [KeyCode_Line1_7] =  'U',
    [KeyCode_Line1_8] =  'I',
    [KeyCode_Line1_9] =  'O',
    [KeyCode_Line1_10] = 'P',

    [KeyCode_Line2_1] =  'A',
    [KeyCode_Line2_2] =  'S',
    [KeyCode_Line2_3] =  'D',
    [KeyCode_Line2_4] =  'F',
    [KeyCode_Line2_5] =  'G',
    [KeyCode_Line2_6] =  'H',
    [KeyCode_Line2_7] =  'J',
    [KeyCode_Line2_8] =  'K',
    [KeyCode_Line2_9] =  'L',

    [KeyCode_Line3_1] =  'Z',
    [KeyCode_Line3_2] =  'X',
    [KeyCode_Line3_3] =  'C',
    [KeyCode_Line3_4] =  'V',
    [KeyCode_Line3_5] =  'B',
    [KeyCode_Line3_6] =  'N',
    [KeyCode_Line3_7] =  'M',

    [KeyCode_Backtick] = '~',
    [KeyCode_Minus] =  '_',
    [KeyCode_Equals] = '+',

    [KeyCode_BracketL] = '{',
    [KeyCode_BracketR] = '}',

    [KeyCode_Semicolon] = ';',
    [KeyCode_Quote] =     '\"',
    [KeyCode_Backslash] = '|',

    [KeyCode_Comma] =  '<',
    [KeyCode_Period] = '>',
    [KeyCode_Slash] =  '?',

    [KeyCode_Return] = '\n',
    [KeyCode_Space] = ' '
};