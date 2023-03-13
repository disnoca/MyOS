#include "drivers/vga/vga.h"
#include "drivers/serial_port/serial_port.h"
#include "gdt.h"
#include "segmentation.h"
#include "idt.h"
#include "libs/parse.h"

int kmain() {
	cursor_init();
	serial_init();

	gdt_init();
	gdt_load();
	load_kernel_segments();

	idt_init();
	idt_load();

	fb_write("Finished Loading.\n");

	return 0;
}