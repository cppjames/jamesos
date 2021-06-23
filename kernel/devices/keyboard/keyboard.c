#include <devices/keyboard.h>

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

static void setKeyboardScanCode();

static void updateState(Key key);
static void updateModifiers(Key key);
static void updateKeySwitches(Key key);
static void sendState();

static size_t nextEventQueuePos(size_t pos);

static KeyEvent keyboard_state = {
    .key = { 0 },
    .flags = 0x0
};

static EventQueue event_queue = {
	.buffer = {},
	.current = 0,
	.end = 0
};


void initKeyboard() {
    // TODO - Bind IRQ handler here
    setKeyboardScanCode();
}


void sendKey(Key key) {
    updateState(key);
    sendState();
}


bool isKeyEvent() {
    return event_queue.current != event_queue.end;
}


KeyEvent getKeyEvent() {
    // There is a key event in the queue
    if (isKeyEvent()) {
        size_t current = event_queue.current;
		KeyEvent ev = event_queue.buffer[current];
		event_queue.current = nextEventQueuePos(current);
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


void setKeyboardScanCode() {
    while (true) {
        outb(CHANGE_SCANCODE_SET, KEYBOARD_PORT);
        outb(SCANCODE_SET_1, KEYBOARD_PORT);

        unsigned char response = inb(KEYBOARD_PORT);

        // Response is not bad, we can stop now
        if (response != CHANGE_SCANCODE_SET_FAIL)
            break;
    }
}


void updateState(Key key) {
    updateModifiers(key);
    updateKeySwitches(key);
    keyboard_state.key = key;
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
        keyboard_state.flags |= flag;
    else
        keyboard_state.flags &= ~flag;
}


void updateKeySwitches(Key key) {
    if (isCapsLock(key) && key.press)
        keyboard_state.flags ^= KeyFlag_CapsLock;
}


void sendState() {
    event_queue.buffer[event_queue.end] = keyboard_state;
	event_queue.end = nextEventQueuePos(event_queue.end);
}


static inline size_t nextEventQueuePos(size_t pos) {
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