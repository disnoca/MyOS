#include <kernel/tty.h>
#include "../drivers/vga/vga.h"

void tty_init(void) {
	vga_init();
}

void tty_putchar(char c) {
	fb_writechar(c);
}

void tty_write(const char* data, size_t size) {
	fb_write(data, size);
}

void tty_writestring(const char* data) {
	fb_writestring(data);
}