#ifndef KEYBOARD_INTERNAL_H
#define KEYBOARD_INTERNAL_H

#include <devices/keyboard.h>

#define QUEUE_SIZE 4096

typedef struct event_queue_t {
	key_event_t buffer[QUEUE_SIZE];
	size_t current;
	size_t end;
} event_queue_t;

void _updateStateMachine(key_t key);
void _updateModifiers(key_t key);
void _updateKeySwitches(key_t key);

void _sendStateMachine();

inline static size_t nextEventQueuePos(size_t pos) {
	return (pos + 1) % QUEUE_SIZE;
}

#endif // KEYBOARD_INTERNAL_H