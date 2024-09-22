#include <string.h>

int strncmp(const char* str1, const char* str2, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (str1[i] != str2[i]) return str1[i] - str2[i];
		if (!str1[i]) break;
	}
	return 0;
}
