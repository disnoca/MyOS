#include <kernel/system.h>
#include "../drivers/vga/vga.h"

void panic(const char* msg, const char* file, int line)
{
	IRQ_OFF;

	fb_clear();
	fb_writestring(msg);
	fb_writestring(" in ");
	fb_writestring(file);
	fb_writestring(" at line ");
	//fb_writestring(line);
	cursor_disable();

	STOP;
}
