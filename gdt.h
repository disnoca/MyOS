#ifndef GDT
#define GDT

/**	init_gdt:
 * 	Encodes the Global Descriptor Table for Flat-Memory Mode (no segmentation)
*/
void init_gdt(void);

/**	load_gdt:
 * 	Loads the Global Descriptor Table into the Global Descriptor Table Register
*/
void load_gdt(void);

#endif