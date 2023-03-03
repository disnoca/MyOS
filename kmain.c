#include "libs/string.h"
#include "drivers/vga/vga.h"
#include "drivers/serial_port/serial_port.h"

int kmain() {
	init_cursor();
	serial_init();

	return 0;
}