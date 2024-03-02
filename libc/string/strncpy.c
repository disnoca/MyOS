#include <string.h>

char* strncpy(char* restrict dststr, const char* restrict srcstr, size_t size) {
	for(size_t i = 0; i < size; i++) {
		dststr[i] = srcstr[i];
		if(!srcstr[i]) break;
	}
	return dststr;
}
