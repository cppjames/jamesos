#ifndef FIFO_H
#define FIFO_H

#include <stdbool.h>
#include <stddef.h>

void push(char c);
char pop();
bool isThereMore();

#endif // FIFO_H
