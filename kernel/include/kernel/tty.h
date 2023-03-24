#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

/**	tty_init:
 *	Initializes the TTY.
*/
void tty_init(void);

/**	tty_putchar:
 *	Writes a charaacter to the TTY.
*/
void tty_putchar(char c);

/** tty_write:
 *	Writes size bytes from the given data to the TTY.
*/
void tty_write(const char* data, size_t size);

/**	tty_writestring
 *	Write the given string to the TTY.
*/
void tty_writestring(const char* data);

#endif
