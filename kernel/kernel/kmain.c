#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/segmentation.h>
#include <kernel/pic.h>
#include <kernel/idt.h>
#include <kernel/paging.h>

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

	paging_enable();
	printf("Enabled Paging\n");

	printf("Finished Loading\n");

	return 0;
}
