#include <kernel/multiboot.h>
#include <kernel/arch/i386/system.h>
#include <kernel/arch/i386/vga.h>
#include <kernel/arch/i386/serial.h>
#include <kernel/arch/i386/mm.h>
#include <kernel/arch/i386/pic.h>

#include <stdio.h>
#include <stdint.h>


extern void gdt_init(void);
extern void idt_init(void);

extern void bmap_print(void);


int kmain(multiboot_info_t* mbi, uint32_t magic)
{
	vga_init();
	serial_init();

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		PANIC("Invalid magic number");
	}

	mm_init(mbi);
	printf("Detected Memory\n");
	printf("Initialized Page Allocator\n");
	printf("Initialized Slab Allocator\n");

	gdt_init();
	printf("Loaded GDT\n");

	pic_init();
	printf("Initialized PIC\n");

	idt_init();
	printf("Loaded IDT\n");

	printf("Finished Loading\n");

	return 0;
}
