#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

/**
 * Encodes the Global Descriptor Table for Flat-Memory Model (no segmentation) and
 * loads the Global Descriptor Table into the Global Descriptor Table Register.
*/
void gdt_init(void);

#endif
