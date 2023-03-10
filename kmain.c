#include "drivers/vga/vga.h"
#include "drivers/serial_port/serial_port.h"
#include "gdt.h"

int kmain() {
	init_cursor();
	serial_init();

	init_gdt();
	load_gdt();

	return 0;
}