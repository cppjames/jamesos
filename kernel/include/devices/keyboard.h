#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define KEYBOARD_PORT 0x60

typedef enum KeyCode {
    KeyCode_None,
    KeyCode_Escape,
    KeyCode_1,
    KeyCode_2,
    KeyCode_3,
    KeyCode_4,
    KeyCode_5,
    KeyCode_6,
    KeyCode_7,
    KeyCode_8,
    KeyCode_9,
    KeyCode_0,
    KeyCode_Minus,
    KeyCode_Equals,
    KeyCode_Backspace,
    KeyCode_Tab,
    KeyCode_Line1_1,
    KeyCode_Line1_2,
    KeyCode_Line1_3,
    KeyCode_Line1_4,
    KeyCode_Line1_5,
    KeyCode_Line1_6,
    KeyCode_Line1_7,
    KeyCode_Line1_8,
    KeyCode_Line1_9,
    KeyCode_Line1_10,
    KeyCode_BracketL,
    KeyCode_BracketR,
    KeyCode_Return,
    KeyCode_CtrlL,
    KeyCode_Line2_1,
    KeyCode_Line2_2,
    KeyCode_Line2_3,
    KeyCode_Line2_4,
    KeyCode_Line2_5,
    KeyCode_Line2_6,
    KeyCode_Line2_7,
    KeyCode_Line2_8,
    KeyCode_Line2_9,
    KeyCode_Semicolon,
    KeyCode_Quote,
    KeyCode_Backtick,
    KeyCode_ShiftL,
    KeyCode_Backslash,
    KeyCode_Line3_1,
    KeyCode_Line3_2,
    KeyCode_Line3_3,
    KeyCode_Line3_4,
    KeyCode_Line3_5,
    KeyCode_Line3_6,
    KeyCode_Line3_7,
    KeyCode_Comma,
    KeyCode_Period,
    KeyCode_Slash,
    KeyCode_ShiftR,
    KeyCode_Asterix,
    KeyCode_AltL,
    KeyCode_Space,
    KeyCode_CapsLock,
    KeyCode_F1,
    KeyCode_F2,
    KeyCode_F3,
    KeyCode_F4,
    KeyCode_F5,
    KeyCode_F6,
    KeyCode_F7,
    KeyCode_F8,
    KeyCode_F9,
    KeyCode_F10,
    KeyCode_F11,
    KeyCode_F12,
    KeyCode_PrevTrack,
    KeyCode_NextTrack,
    KeyCode_CtrlR,
    KeyCode_Mute,
    KeyCode_Calculator,
    KeyCode_Play,
    KeyCode_Stop,
    KeyCode_VolumeDown,
    KeyCode_VolumeUp,
    KeyCode_WebHome,
    KeyCode_AltR,
    KeyCode_Home,
    KeyCode_UpArrow,
    KeyCode_PageUp,
    KeyCode_LeftArrow,
    KeyCode_RightArrow,
    KeyCode_End,
    KeyCode_DownArrow,
    KeyCode_PageDown,
    KeyCode_Insert,
    KeyCode_Delete,
    KeyCode_LeftGUI,
    KeyCode_RightGUI,
    KeyCode_Apps,
    KeyCode_Power,
    KeyCode_Sleep,
    KeyCode_Wake,
    KeyCode_WebSearch,
    KeyCode_WebFavorites,
    KeyCode_WebRefresh,
    KeyCode_WebStop,
    KeyCode_WebForward,
    KeyCode_WebBack,
    KeyCode_MyComputer,
    KeyCode_Email,
    KeyCode_MediaSelect,
    KeyCode_PrintScreen,
    KeyCode_Pause
} KeyCode;

typedef enum KeyFlag {
    KeyFlag_IsKey =     0x01,
    KeyFlag_Shift =     0x02,
    KeyFlag_Control =   0x04,
    KeyFlag_Alt =       0x08,
    KeyFlag_CapsLock =  0x10
} KeyFlag;

typedef struct Key {
    KeyCode code;
    bool press;
    bool keypad;
} Key;

typedef struct KeyEvent {
    Key key;
    uint16_t flags;
} KeyEvent;

extern const char KEY_ASCII_MAP[];
extern const char KEY_ASCII_MAP_UPPER[];

void initKeyboard();

Key codeToKey(unsigned char code);
void sendKey(Key key);

bool isKeyEvent();
KeyEvent getKeyEvent();
bool keyEventUpper(KeyEvent event);
unsigned char keyEventToAscii(KeyEvent event);

#define keyToAscii(key)      KEY_ASCII_MAP[(key).code]
#define keyToAsciiUpper(key) KEY_ASCII_MAP_UPPER[(key).code]

#define isLetter(key) isalpha(keyToAscii(key))
#define isDigit(key)  isdigit(keyToAscii(key))
#define isAlnum(key)  isalnum(keyToAscii(key))
#define isAscii(key)  (!!keyToAscii(key))

#define isShift(key)    ((key).code == KeyCode_ShiftL || (key).code == KeyCode_ShiftR)
#define isCtrl(key)     ((key).code == KeyCode_CtrlL  || (key).code == KeyCode_CtrlR)
#define isAlt(key)      ((key).code == KeyCode_AltL   || (key).code == KeyCode_AltR)
#define isMeta(key)     ((key).code == KeyCode_Meta)
#define isCapsLock(key) ((key).code == KeyCode_CapsLock)

#endif // KEYBOARD_H