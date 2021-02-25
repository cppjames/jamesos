#ifndef KEYBOARD_INTERNAL_H
#define KEYBOARD_INTERNAL_H

#include <devices/keyboard.h>

#define QUEUE_SIZE 4096

typedef struct EventQueue {
	KeyEvent buffer[QUEUE_SIZE];
	size_t current;
	size_t end;
} EventQueue;

void _updateStateMachine(Key key);
void _updateModifiers(Key key);
void _updateKeySwitches(Key key);

void _sendStateMachine();

inline static size_t nextEventQueuePos(size_t pos) {
	return (pos + 1) % QUEUE_SIZE;
}

#endif // KEYBOARD_INTERNAL_H