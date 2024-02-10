#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/segmentation.h>
#include <kernel/pic.h>
#include <kernel/idt.h>
#include <kernel/memory.h>
#include <kernel/multiboot.h>
#include <kernel/system.h>
#include "../drivers/vga/vga.h"
#include "../drivers/serial/serial.h"

#include <stdio.h>
#include <stdint.h>

int kmain(multiboot_info_t* mbd, uint32_t magic)
{
	vga_init();
	serial_init();

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("Invalid magic number");
	}

	/* Check if there's a valid memory map */
    if(!(mbd->flags & MULTIBOOT_INFO_MEM_MAP)) {
        PANIC("Invalid memory map given by GRUB bootloader");
	}

	memory_detect(mbd);
	printf("Detected Memory\n");

	gdt_init();
	load_kernel_segments();
	printf("Loaded GDT\n");

	pic_init();
	printf("Initialized PIC\n");

	idt_init();
	printf("Loaded IDT\n");

	printf("Finished Loading\n");

	return 0;
}
