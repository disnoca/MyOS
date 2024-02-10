#include <kernel/system.h>
#include <stdio.h>
#include "../drivers/vga/vga.h"

void panic(const char* msg, const char* file, int line)
{
	IRQ_OFF;

	vga_clear();
	printf("%s in %s at line %d\n", msg, file, line);
	cursor_disable();

	STOP;
}
