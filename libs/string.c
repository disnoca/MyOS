#include "string.h"

size_t strlen(char* str) {
	unsigned len = 0;
	char c;

	do {
		c = str[len++];
	} while(c != '\0');

	return len;
}