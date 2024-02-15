#include <kernel/tty.h>

#ifdef __i386__
#include <kernel/arch/i386/vga.h>
#include <kernel/arch/i386/serial.h>
#endif

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
