#include "string.h"

unsigned int strlen(char* str) {
	unsigned len = 0;
	char c;

	do {
		c = str[len++];
	} while(c != '\0');

	return len;
}