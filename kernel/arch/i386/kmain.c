#include <kernel/multiboot.h>
#include <kernel/arch/i386/system.h>
#include <kernel/arch/i386/vga.h>
#include <kernel/arch/i386/serial.h>
#include <kernel/arch/i386/memory.h>
#include <kernel/arch/i386/pic.h>

#include <stdio.h>
#include <stdint.h>


extern void gdt_init(void);
extern void idt_init(void);


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
	printf("Loaded GDT\n");

	pic_init();
	printf("Initialized PIC\n");

	idt_init();
	printf("Loaded IDT\n");

	printf("Finished Loading\n");

	return 0;
}
