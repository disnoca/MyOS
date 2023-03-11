#ifndef GDT
#define GDT

/**	gdt_init:
 * 	Encodes the Global Descriptor Table for Flat-Memory Mode (no segmentation)
*/
void gdt_init(void);

/**	gdt_load:
 * 	Loads the Global Descriptor Table into the Global Descriptor Table Register
*/
void gdt_load(void);

#endif