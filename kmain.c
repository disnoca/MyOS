#include "drivers/vga/vga.h"
#include "drivers/serial_port/serial_port.h"
#include "gdt.h"
#include "segmentation.h"
#include "idt.h"

int kmain() {
	cursor_init();
	serial_init();

	gdt_init();
	gdt_load();
	load_kernel_segments();

	idt_init();
	idt_load();

	return 0;
}