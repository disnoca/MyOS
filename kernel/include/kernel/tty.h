#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

/**
 *	Writes a charaacter to the TTY.
*/
void tty_putchar(char c);

/**
 *	Writes size bytes from the given data to the TTY.
*/
void tty_write(const char* data, size_t size);

/**
 *	Write the given string to the TTY.
*/
void tty_writestring(const char* data);

#endif
