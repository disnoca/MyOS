#include <string.h>

char* strcpy(char* restrict dststr, const char* restrict srcstr) {
	for (size_t i = 0; ; i++) {
		dststr[i] = srcstr[i];
		if (!srcstr[i]) break;
	}
	return dststr;
}
