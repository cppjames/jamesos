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

static unsigned char scancodeSequenceLength = 0;

static bool isPrintScreenPress = false;
static bool isPrintScreenRelease = false;
static bool isPause = false;

static const KeyCode CODE_TO_KEY_MAP[256];
static const KeyCode MBCODE_TO_KEY_MAP[256];

#define sequenceLength() scancodeSequenceLength
#define setSequenceLength(len) (scancodeSequenceLength = (len))
#define isMultibyte() (sequenceLength() > 0)
#define isMultibyteStart() (sequenceLength() == 1)

static void advanceMultibyteSequence(void);
static void resetSequence(void);

static Key constructKeyFromCode(unsigned char code);
static Key constructKeyFromCodeMB(unsigned char code);
static Key constructEmptyKey(void);
static Key constructPrintScreenPressKey(void);
static Key constructPrintScreenReleaseKey(void);
static Key constructPauseKey(void);


// Convert scancode to key.
//
// For single-byte scancodes, the mapping is trivial.
// All multibyte scancode sequences begin with the same code (MULTIBYTE_BEGIN).
//
// The only keys that use more than 2 scancodes are:
//     - Pause key (can only be pressed)
//     - PrintScreen key press
//     - PrintScreen key release
// Once one of these is detected, there is no need to keep track of the scancodes that follow.
// Each sequence is distinct, so we only need to count the number of scancodes and return the correct key when the sequence ends.
//
// When a sequence has not yet ended, an empty key is returned (which has KeyCode_None as its code).
Key codeToKey(unsigned char code) {
    // We are in the middle of a multibyte sequence
    if (isMultibyte()) {

        // Key is the second of a multibyte sequence
        if (isMultibyteStart()) {

            // Key continues a "print screen" / "pause" sequence
            if (code == PRINT_SCREEN_PRESS_BEGIN   ||
                code == PRINT_SCREEN_RELEASE_BEGIN ||
                code == PAUSE_SECOND_BYTE) {
                
                isPrintScreenPress = (code == PRINT_SCREEN_PRESS_BEGIN);
                isPrintScreenRelease = (code == PRINT_SCREEN_RELEASE_BEGIN);
                isPause = (code == PAUSE_SECOND_BYTE);

                goto nextCode;
            }

            // Multibyte sequence ends here, return corresponding key
            resetSequence();
            return constructKeyFromCodeMB(code);
        } else {
            advanceMultibyteSequence();
            size_t seqLen = sequenceLength();

            // Key is the last of a "print screen" press multibyte sequence
            if (isPrintScreenPress && seqLen == PRINT_SCREEN_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPrintScreenPressKey();
            }

            // Key is the last of a "print screen" release multibyte sequence
            if (isPrintScreenRelease && seqLen == PRINT_SCREEN_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPrintScreenReleaseKey();
            }
            
            // Key is the last of a "pause" press multibyte sequence
            if (isPause && seqLen == PAUSE_SEQUENCE_LENGTH) {
                resetSequence();
                return constructPauseKey();
            }
            
            return constructEmptyKey();
        }
    } else {
        // Key is the beginning of a multibyte sequence
        if (code == MULTIBYTE_BEGIN || code == PAUSE_BEGIN) {
            isPause = (code == PAUSE_BEGIN);
            goto nextCode;
        }

        // Code is single-byte, return corresponding key 
        return constructKeyFromCode(code);
    }

    // Return placeholder key and wait for next code in sequence
nextCode:
    advanceMultibyteSequence();
    return constructEmptyKey();
}


static void advanceMultibyteSequence(void) {
    setSequenceLength(sequenceLength() + 1);
}


static void resetSequence(void) {
    isPause = false;
    isPrintScreenPress = false;
    isPrintScreenRelease = false;
    setSequenceLength(0);
}


static Key constructKeyFromCode(unsigned char code) {
    return (Key) {
        .code = CODE_TO_KEY_MAP[code % 128],
        .press = (code <= 128)
    };
}


static Key constructKeyFromCodeMB(unsigned char code) {
    return (Key) {
        .code = MBCODE_TO_KEY_MAP[code % 128],
        .press = (code <= 128)
    };
}


static Key constructEmptyKey(void) {
    return (Key) {
        .code = KeyCode_None
    };
}


static Key constructPrintScreenPressKey(void) {
    return (Key) {
        .code = KeyCode_PrintScreen,
        .press = true
    };
}


static Key constructPrintScreenReleaseKey(void) {
    return (Key) {
        .code = KeyCode_PrintScreen,
        .press = false
    };
}


static Key constructPauseKey(void) {
    return (Key) {
        .code = KeyCode_Pause,
        .press = true
    };
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