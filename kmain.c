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

	fb_write("Finished Loading. ");

	char str0[2], str1[2], str2[3], str3[10], str4[11];
	str0[1] = '\0';
	str1[1] = '\0';
	str2[2] = '\0';
	str3[9] = '\0';
	str4[10] = '\0';

	parse_unsigned(str0, 0);
	parse_unsigned(str1, 9);
	parse_unsigned(str2, 52);
	parse_unsigned(str3, 193462919);
	parse_unsigned(str4, 4294967295);

	fb_write(str0);
	fb_write(" ");
	fb_write(str1);
	fb_write(" ");
	fb_write(str2);
	fb_write(" ");
	fb_write(str3);
	fb_write(" ");
	fb_write(str4);
	fb_write(" ");

	return 0;
}