#include "fifo.h"

#define BUFSIZE 4096

typedef struct Queue_t {
	char buffer[BUFSIZE];
	size_t current;
	size_t end;
} Queue_t;

static Queue_t queue = (Queue_t) {
	.buffer = { 0 },
	.current = 0,
	.end = 0
};

inline static size_t nextPos(size_t pos) {
	if (pos < BUFSIZE-1)
		return pos + 1;
	return 0;
}

void push(char c) {
	queue.buffer[queue.end] = c;
	queue.end = nextPos(queue.end);
}

char pop() {
	if (isThereMore()) {
		char ch = queue.buffer[queue.current];
		queue.current = nextPos(queue.current);
		return ch;
	}

	return 0;
}

bool isThereMore() {
	return queue.current != queue.end;
}
