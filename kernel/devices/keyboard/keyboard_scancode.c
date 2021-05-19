#include <devices/keyboard.h>

#include <stdbool.h>
#include <stddef.h>

#define MULTIBYTE_BEGIN   0xE0
#define PAUSE_BEGIN       0xE1
#define PAUSE_SECOND_BYTE 0x1D

#define PRINT_SCREEN_PRESS_BEGIN   0x2A
#define PRINT_SCREEN_RELEASE_BEGIN 0xB7

#define PRINT_SCREEN_SEQUENCE_LENGTH 4
#define PAUSE_SEQUENCE_LENGTH        6

static unsigned char scancode_sequence_length;

static bool is_print_screen_press = false;
static bool is_print_screen_release = false;
static bool is_pause = false;

static const KeyCode CODE_TO_KEY_MAP[256];
static const KeyCode MBCODE_TO_KEY_MAP[256];

#define sequenceLength() scancode_sequence_length
#define setSequenceLength(len) (scancode_sequence_length = (len))
#define isMultibyte() (sequenceLength() > 0)
#define isMultibyteStart() (sequenceLength() == 1)

static void addMultibyteCode();
static void resetSequence();

static Key constructKeyFromCode(unsigned char code);
static Key constructKeyFromCodeMB(unsigned char code);
static Key constructEmptyKey();
static Key constructPrintScreenPressKey();
static Key constructPrintScreenReleaseKey();
static Key constructPauseKey();


Key codeToKey(unsigned char code) {
    if (isMultibyte()) {
        if (isMultibyteStart()) {
            if (code == PRINT_SCREEN_PRESS_BEGIN   ||
                code == PRINT_SCREEN_RELEASE_BEGIN ||
                code == PAUSE_SECOND_BYTE) {
                
                is_print_screen_press = (code == PRINT_SCREEN_PRESS_BEGIN);
                is_print_screen_release = (code == PRINT_SCREEN_RELEASE_BEGIN);
                is_pause = (code == PAUSE_SECOND_BYTE);

                goto next_code;
            }

            resetSequence();
            return constructKeyFromCodeMB(code);
        } else {
            addMultibyteCode();
            size_t seqLen = sequenceLength();

            if (is_print_screen_press && seqLen == PRINT_SCREEN_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPrintScreenPressKey();
            }

            if (is_print_screen_release && seqLen == PRINT_SCREEN_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPrintScreenReleaseKey();
            }
            
            if (is_pause && seqLen == PAUSE_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPauseKey();
            }
            
            return constructEmptyKey();
        }
    } else {
        if (code == MULTIBYTE_BEGIN || code == PAUSE_BEGIN) {
            is_pause = (code == PAUSE_BEGIN);
            goto next_code;
        }

        return constructKeyFromCode(code);
    }

next_code:
    addMultibyteCode();
    return constructEmptyKey();
}


static inline void addMultibyteCode() {
    setSequenceLength(sequenceLength() + 1);
}


static inline void resetSequence() {
    is_pause = false;
    is_print_screen_press = false;
    is_print_screen_release = false;
    setSequenceLength(0);
}


static inline Key constructKeyFromCode(unsigned char code) {
    return (Key) { .code = CODE_TO_KEY_MAP[code % 128], .press = (code <= 128) };
}


static inline Key constructKeyFromCodeMB(unsigned char code) {
    return (Key) { .code = MBCODE_TO_KEY_MAP[code % 128], .press = (code <= 128) };
}


static inline Key constructEmptyKey() {
    return (Key) { .code = KeyCode_None };
}


static inline Key constructPrintScreenPressKey() {
    return (Key) { .code = KeyCode_PrintScreen, .press = true };
}


static inline Key constructPrintScreenReleaseKey() {
    return (Key) { .code = KeyCode_PrintScreen, .press = false };
}


static inline Key constructPauseKey() {
    return (Key) { .code = KeyCode_Pause, .press = true };
}


static const KeyCode CODE_TO_KEY_MAP[] = {
    [0x00] = KeyCode_None,
    [0x01] = KeyCode_Escape,
    [0x02] = KeyCode_1,
    [0x03] = KeyCode_2,
    [0x04] = KeyCode_3,
    [0x05] = KeyCode_4,
    [0x06] = KeyCode_5,
    [0x07] = KeyCode_6,
    [0x08] = KeyCode_7,
    [0x09] = KeyCode_8,
    [0x0A] = KeyCode_9,
    [0x0B] = KeyCode_0,
    [0x0C] = KeyCode_Minus,
    [0x0D] = KeyCode_Equals,
    [0x0E] = KeyCode_Backspace,
    [0x0F] = KeyCode_Tab,
    [0x10] = KeyCode_Line1_1,
    [0x11] = KeyCode_Line1_2,
    [0x12] = KeyCode_Line1_3,
    [0x13] = KeyCode_Line1_4,
    [0x14] = KeyCode_Line1_5,
    [0x15] = KeyCode_Line1_6,
    [0x16] = KeyCode_Line1_7,
    [0x17] = KeyCode_Line1_8,
    [0x18] = KeyCode_Line1_9,
    [0x19] = KeyCode_Line1_10,
    [0x1A] = KeyCode_BracketL,
    [0x1B] = KeyCode_BracketR,
    [0x1C] = KeyCode_Return,
    [0x1D] = KeyCode_CtrlL,
    [0x1E] = KeyCode_Line2_1,
    [0x1F] = KeyCode_Line2_2,
    [0x20] = KeyCode_Line2_3,
    [0x21] = KeyCode_Line2_4,
    [0x22] = KeyCode_Line2_5,
    [0x23] = KeyCode_Line2_6,
    [0x24] = KeyCode_Line2_7,
    [0x25] = KeyCode_Line2_8,
    [0x26] = KeyCode_Line2_9,
    [0x27] = KeyCode_Semicolon,
    [0x28] = KeyCode_Quote,
    [0x29] = KeyCode_Backtick,
    [0x2A] = KeyCode_ShiftL,
    [0x2B] = KeyCode_Backslash,
    [0x2C] = KeyCode_Line3_1,
    [0x2D] = KeyCode_Line3_2,
    [0x2E] = KeyCode_Line3_3,
    [0x2F] = KeyCode_Line3_4,
    [0x30] = KeyCode_Line3_5,
    [0x31] = KeyCode_Line3_6,
    [0x32] = KeyCode_Line3_7,
    [0x33] = KeyCode_Comma,
    [0x34] = KeyCode_Period,
    [0x35] = KeyCode_Slash,
    [0x36] = KeyCode_ShiftR,
    [0x37] = KeyCode_Asterix,
    [0x38] = KeyCode_AltL,
    [0x39] = KeyCode_Space,
    [0x3A] = KeyCode_CapsLock,
    [0x3B] = KeyCode_F1,
    [0x3C] = KeyCode_F2,
    [0x3D] = KeyCode_F3,
    [0x3E] = KeyCode_F4,
    [0x3F] = KeyCode_F5,
    [0x40] = KeyCode_F6,
    [0x41] = KeyCode_F7,
    [0x42] = KeyCode_F8,
    [0x43] = KeyCode_F9,
    [0x44] = KeyCode_F10,
    [0x57] = KeyCode_F11,
    [0x58] = KeyCode_F12
};

static const KeyCode MBCODE_TO_KEY_MAP[] = {
    [0x10] = KeyCode_PrevTrack,
    [0x19] = KeyCode_NextTrack,
    [0x1D] = KeyCode_CtrlR,
    [0x20] = KeyCode_Mute,
    [0x21] = KeyCode_Calculator,
    [0x22] = KeyCode_Play,
    [0x24] = KeyCode_Stop,
    [0x2E] = KeyCode_VolumeDown,
    [0x30] = KeyCode_VolumeUp,
    [0x32] = KeyCode_WebHome,
    [0x38] = KeyCode_AltR,
    [0x47] = KeyCode_Home,
    [0x48] = KeyCode_UpArrow,
    [0x49] = KeyCode_PageUp,
    [0x4B] = KeyCode_LeftArrow,
    [0x4D] = KeyCode_RightArrow,
    [0x4F] = KeyCode_End,
    [0x50] = KeyCode_DownArrow,
    [0x51] = KeyCode_PageDown,
    [0x52] = KeyCode_Insert,
    [0x53] = KeyCode_Delete,
    [0x5B] = KeyCode_LeftGUI,
    [0x5C] = KeyCode_RightGUI,
    [0x5D] = KeyCode_Apps,
    [0x5E] = KeyCode_Power,
    [0x5F] = KeyCode_Sleep,
    [0x63] = KeyCode_Wake,
    [0x65] = KeyCode_WebSearch,
    [0x66] = KeyCode_WebFavorites,
    [0x67] = KeyCode_WebRefresh,
    [0x68] = KeyCode_WebStop,
    [0x69] = KeyCode_WebForward,
    [0x6A] = KeyCode_WebBack,
    [0x6B] = KeyCode_MyComputer,
    [0x6C] = KeyCode_Email,
    [0x6D] = KeyCode_MediaSelect
};