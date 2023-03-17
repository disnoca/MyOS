#include "drivers/vga/vga.h"
#include "drivers/serial_port/serial_port.h"
#include "libs/parse.h"
#include "gdt.h"
#include "segmentation.h"
#include "idt.h"
#include "pic.h"

int kmain() {
	vga_init();
	serial_init();

	gdt_init();
	gdt_load();
	load_kernel_segments();
	fb_write("Loaded GDT\n");

	pic_init();
	fb_write("Initialized PIC\n");

	idt_init();
	idt_load();
	fb_write("Loaded IDT\n");

	fb_write("Finished Loading\n");

	return 0;
}