#include <kernel/vga.h>
#include <kernel/gdt.h>
#include <kernel/segmentation.h>
#include <kernel/pic.h>
#include <kernel/idt.h>

int kmain() {
	vga_init();

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