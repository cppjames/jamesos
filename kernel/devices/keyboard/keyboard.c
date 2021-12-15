#include <devices/keyboard.h>

#include <kernel/system/idt.h>

#include <stddef.h>
#include <stdio.h>
#include <sys/io.h>

#define QUEUE_SIZE 4096

#define CHANGE_SCANCODE_SET        0xF0
#define SCANCODE_SET_1             0x01
#define CHANGE_SCANCODE_SET_FAIL   0xFE

typedef struct EventQueue {
	KeyEvent buffer[QUEUE_SIZE];
	size_t current;
	size_t end;
} EventQueue;

void keyboardIrqHandler(void);
static void setKeyboardScanCode(void);

static void updateState(Key key);
static void updateModifiers(Key key);
static void updateKeyToggles(Key key);
static void sendState(void);

static size_t nextEventQueuePos(size_t pos);

static KeyEvent keyboardState = {
    .key = { 0 },
    .flags = 0x0
};

static EventQueue eventQueue = {
	.buffer = {},
	.current = 0,
	.end = 0
};


void initKeyboard(void) {
    registerIrqHandler(Irq_KeyboardInterrupt, keyboardIrqHandler);
    setKeyboardScanCode();
    clearIrqMask(Irq_KeyboardInterrupt);
}


void keyboardIrqHandler(void) {
    unsigned char scanCode = inb(KEYBOARD_PORT);

    Key key = codeToKey(scanCode);

    // Either the scancode is invalid, or we are in the middle of a multibyte scancode sequence.
    if (key.code == KeyCode_None)
        return;

    sendKey(key);

    // Keyboard does not send pause release scancode, so we have to simulate it.
    // The pause key is assumed to be released as soon as it is pressed.
    if (key.code == KeyCode_Pause) {
        Key keyRelease = key;
        keyRelease.press = false;
        sendKey(keyRelease);
    }
}


void sendKey(Key key) {
    updateState(key);
    sendState();
}


bool isKeyEvent(void) {
    return eventQueue.current != eventQueue.end;
}


KeyEvent getKeyEvent(void) {
    // There is a key event in the queue
    if (isKeyEvent()) {
        size_t current = eventQueue.current;
		KeyEvent ev = eventQueue.buffer[current];
		eventQueue.current = nextEventQueuePos(current);
		return ev;
	}

	return (KeyEvent) {
        .key = { .code = KeyCode_None },
        .flags = 0x0
    };
}


bool keyEventUpper(KeyEvent event) {
    // We do a little boolean trolling
    bool capslock = event.flags & KeyFlag_CapsLock;
    bool shift = event.flags & KeyFlag_Shift;
    bool upper = capslock && isLetter(event.key);
    return shift ? !upper : upper;
}


unsigned char keyEventToAscii(KeyEvent event) {
    return keyEventUpper(event) ? keyToAsciiUpper(event.key) : keyToAscii(event.key);
}


void setKeyboardScanCode(void) {
    while (true) {
        outb(CHANGE_SCANCODE_SET, KEYBOARD_PORT);
        outb(SCANCODE_SET_1, KEYBOARD_PORT);

        unsigned char response = inb(KEYBOARD_PORT);

        // Request did not fail, we can stop now
        if (response != CHANGE_SCANCODE_SET_FAIL)
            break;
    }
}


void updateState(Key key) {
    updateModifiers(key);
    updateKeyToggles(key);
    keyboardState.key = key;
}


void updateModifiers(Key key) {
    KeyFlag flag = 0x0;

    if      (isShift(key)) flag = KeyFlag_Shift;
    else if (isCtrl(key))  flag = KeyFlag_Control;
    else if (isAlt(key))   flag = KeyFlag_Alt;

    if (!flag)
        return;

    // Set modifier on press and clear on release
    if (key.press)
        keyboardState.flags |= flag;
    else
        keyboardState.flags &= ~flag;
}


void updateKeyToggles(Key key) {
    // Toggle caps lock flag using bitwise xor
    if (isCapsLock(key) && key.press)
        keyboardState.flags ^= KeyFlag_CapsLock;
}

// Push keyboard state to key event queue.
void sendState(void) {
    eventQueue.buffer[eventQueue.end] = keyboardState;
	eventQueue.end = nextEventQueuePos(eventQueue.end);
}

// Get circular queue index that follows position.
static size_t nextEventQueuePos(size_t pos) {
	return (pos + 1) % QUEUE_SIZE;
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

    [KeyCode_Semicolon] = ':',
    [KeyCode_Quote] =     '\"',
    [KeyCode_Backslash] = '|',

    [KeyCode_Comma] =  '<',
    [KeyCode_Period] = '>',
    [KeyCode_Slash] =  '?',

    [KeyCode_Return] = '\n',
    [KeyCode_Space] = ' '
};