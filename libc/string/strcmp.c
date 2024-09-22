#include <string.h>

int strcmp(const char* str1, const char* str2) {
	for (size_t i = 0; ; i++) {
		if (str1[i] != str2[i]) return str1[i] - str2[i];
		if (!str1[i]) break;
	}
	return 0;
}
