#include <stdio.h>
#include "fifo.h"

int main() {
	push('b');
	push('r');
	push('u');
	push('h');

	while (isThereMore())
		printf("%c", pop());

	return 0;
}
