#include <kernel/tty.h>
#include "../drivers/vga/vga.h"
#include "../drivers/serial/serial.h"

void tty_putchar(char c)
{
	vga_writechar(c);
}

void tty_write(const char* data, size_t size)
{
	vga_write(data, size);
}

void tty_writestring(const char* data)
{
	vga_writestring(data);
}
