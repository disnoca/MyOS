#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/segmentation.h>
#include <kernel/pic.h>
#include <kernel/idt.h>

#include <stdio.h>

int kmain()
{
	tty_init();
	printf("Initialized TTY\n");

	gdt_init();
	gdt_load();
	load_kernel_segments();
	printf("Loaded GDT\n");

	pic_init();
	printf("Initialized PIC\n");

	idt_init();
	idt_load();
	printf("Loaded IDT\n");

	printf("Finished Loading\n");

	return 0;
}
