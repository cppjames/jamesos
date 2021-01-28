#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

typedef enum {
    KeyCode_Escape =        1,
    KeyCode_1 =             2,
    KeyCode_2 =             3,
    KeyCode_3 =             4,
    KeyCode_4 =             5,
    KeyCode_5 =             6,
    KeyCode_6 =             7,
    KeyCode_7 =             8,
    KeyCode_8 =             9,
    KeyCode_9 =             10,
    KeyCode_0 =             11,
    KeyCode_Minus =         12,
    KeyCode_Equals =        13,
    KeyCode_Backspace =     14,
    KeyCode_Tab =           15,
    KeyCode_Line1_1 =       16,
    KeyCode_Line1_2 =       17,
    KeyCode_Line1_3 =       18,
    KeyCode_Line1_4 =       19,
    KeyCode_Line1_5 =       20,
    KeyCode_Line1_6 =       21,
    KeyCode_Line1_7 =       22,
    KeyCode_Line1_8 =       23,
    KeyCode_Line1_9 =       24,
    KeyCode_Line1_10 =      25,
    KeyCode_BracketL =      26,
    KeyCode_BracketR =      27,
    KeyCode_Return =        28,
    KeyCode_Ctrl =          29,
    KeyCode_Line2_1 =       30,
    KeyCode_Line2_2 =       31,
    KeyCode_Line2_3 =       32,
    KeyCode_Line2_4 =       33,
    KeyCode_Line2_5 =       34,
    KeyCode_Line2_6 =       35,
    KeyCode_Line2_7 =       36,
    KeyCode_Line2_8 =       37,
    KeyCode_Line2_9 =       38,
    KeyCode_Semicolon =     39,
    KeyCode_Quote =         40,
    KeyCode_Backtick =      41,
    KeyCode_ShiftL =        42,
    KeyCode_Backslash =     43,
    KeyCode_Line3_1 =       44,
    KeyCode_Line3_2 =       45,
    KeyCode_Line3_3 =       46,
    KeyCode_Line3_4 =       47,
    KeyCode_Line3_5 =       48,
    KeyCode_Line3_6 =       49,
    KeyCode_Line3_7 =       50,
    KeyCode_Comma =         51,
    KeyCode_Period =        52,
    KeyCode_Slash =         53,
    KeyCode_ShiftR =        54,
    KeyCode_Asterix =       55,
    KeyCode_Alt =           56,
    KeyCode_Space =         57,
    KeyCode_CapsLock =      58,
} key_code_t;

typedef enum {
    KeyFlag_IsKey =     0x01,
    KeyFlag_Shift =     0x02,
    KeyFlag_Control =   0x04,
    KeyFlag_Alt =       0x08,
    KeyFlag_CapsLock =  0x10
} key_flag_t;

typedef struct {
    key_code_t code;
    bool press;
} key_t;

typedef struct {
    key_t key;
    uint16_t flags;
} keyboard_state_t;

extern keyboard_state_t keyboard_state;
extern const char KEY_ASCII_MAP[];
extern const char KEY_ASCII_MAP_UPPER[];

key_t codeToKey(unsigned char code);
void sendKey(key_t key);

#define keyToAscii(key)      KEY_ASCII_MAP[(key).code]
#define keyToAsciiUpper(key) KEY_ASCII_MAP_UPPER[(key).code]

#define isLetter(key) isalpha(keyToAscii(key))
#define isDigit(key)  isdigit(keyToAscii(key))
#define isAlnum(key)  isalnum(keyToAscii(key))
#define isAscii(key)  (!!keyToAscii(key))

#define isShift(key)    ((key).code == KeyCode_ShiftL || (key).code == KeyCode_ShiftR)
#define isCtrl(key)     ((key).code == KeyCode_Ctrl)
#define isAlt(key)      ((key).code == KeyCode_Alt)
#define isMeta(key)     ((key).code == KeyCode_Meta)
#define isCapsLock(key) ((key).code == KeyCode_CapsLock)

#endif // KEYBOARD_H