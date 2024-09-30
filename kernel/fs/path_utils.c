/**
 * Auxiliary functions for path string manipulation.
 * 
 * @author Samuel Pires
 */

#include <string.h>

#include <kernel/fs/path_utils.h>


/* Header Implementation */

size_t format_path(const char* src, const char* cwd, char* dst, size_t max_len)
{
	if (!*src || max_len == 0)
		return 0;

	char* dst_start = dst;

	if (*src == PATH_SEPARATOR) {
		src++;
	} else if (cwd[1]) {
		while (*cwd)
			*dst++ = *cwd++;
	}

	do {
		if (src[0] == '.' && (!src[1] || src[1] == PATH_SEPARATOR)) {
			src++;
			continue;
		}
		if (!strncmp(src, "..", 2) && (!src[2] || src[2] == PATH_SEPARATOR)) {
			src += 2;
			if (dst > dst_start)
				do { dst--; } while (*dst != PATH_SEPARATOR);
			continue;
		}

		if ((size_t)(dst - dst_start) >= max_len)
			return 0;
		*dst++ = PATH_SEPARATOR;

		while (*src && *src != PATH_SEPARATOR) {
			if ((size_t)(dst - dst_start) >= max_len)
				return 0;
			*dst++ = *src++;
		}
	} while (*src++ && *src);

	if (dst == dst_start)
		*dst++ = PATH_SEPARATOR;

	if ((size_t)(dst - dst_start) < max_len)
		*dst = '\0';

	return dst - dst_start;
}

bool is_valid_filename(const char* name, const char* forbidden_chars, size_t max_len)
{
	size_t name_len = strlen(name);
	if (name_len == 0 || name_len > max_len)
		return false;

	if (!strcmp(name, ".") || !strcmp(name, ".."))
		return false;

	size_t forbidden_chars_len = strlen(forbidden_chars);

	char c;
	while ((c = *name++))
		for (size_t i = 0; i < forbidden_chars_len; i++)
			if (c == forbidden_chars[i])
				return false;
	
	return true;
}

bool is_valid_path(const char* path, const char* forbidden_chars, size_t max_name_len)
{
	if (!*path)
		return false;

	size_t forbidden_chars_len = strlen(forbidden_chars);

	size_t name_len = 0;
	char c;
	while ((c = *path++)) {
		if (c == PATH_SEPARATOR) {
			name_len = 0;
			continue;
		}
		if (++name_len >= max_name_len)
			return false;
		for (size_t i = 0; i < forbidden_chars_len; i++)
			if (c == forbidden_chars[i])
				return false;
	}

	return true;
}
