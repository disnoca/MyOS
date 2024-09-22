#include <string.h>

char* strrchr(const char* str, int c) {
	const char* last = NULL;
	while (*str) {
		if (*str == c) last = str;
		str++;
	}
	return (char*)last;
}
