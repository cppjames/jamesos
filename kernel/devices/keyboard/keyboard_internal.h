#ifndef KEYBOARD_INTERNAL_H
#define KEYBOARD_INTERNAL_H

#include <devices/keyboard.h>

void _updateStateMachine(key_t key);
void _updateModifiers(key_t key);
void _updateKeySwitches(key_t key);
bool _getStateMachineUpper(key_t key);

#endif // KEYBOARD_INTERNAL_H