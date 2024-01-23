#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

/**
 * 	Encodes the Global Descriptor Table for Flat-Memory Model (no segmentation)
*/
void gdt_init(void);

/**
 * 	Loads the Global Descriptor Table into the Global Descriptor Table Register
*/
void gdt_load(void);

#endif
