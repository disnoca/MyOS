/**
 * Code for setting up the Global Descriptor Table.
 * 
 * Refer to:
 * AMD64 Architecture Programmer's Manual Volume 2: System Programming Section 4
 * https://wiki.osdev.org/Global_Descriptor_Table
 * https://wiki.osdev.org/GDT_Tutorial
 * 
 * @author Samuel Pires
*/

#include "gdt.h"
#include "tss.h"
#include "types.h"

#define FLAT_MODE_BASE 							0x0
#define FLAT_MODE_LIMIT 						0xFFFFF

#define KERNEL_MODE_CODE_SEGMENT_ACCESS_BYTE 	0x9A
#define KERNEL_MODE_DATA_SEGMENT_ACCESS_BYTE 	0x92
#define USER_MODE_CODE_SEGMENT_ACCESS_BYTE	 	0xFA
#define USER_MODE_DATA_SEGMENT_ACCESS_BYTE	 	0xF2
#define TASK_STATE_SEGMENT_ACCESS_BYTE 			0x89

#define LEGACY_MODE_SEGMENT_FLAGS     			0xC

struct gdt {
	uint8_t null_descriptor[8];
	uint8_t kernel_mode_code_segment[8];
	uint8_t kernel_mode_data_segment[8];
	uint8_t user_mode_code_segment[8];
	uint8_t user_mode_data_segment[8];
	uint8_t task_state_segment[8];
} __attribute__((packed));

struct gdt_descriptor {
	uint32_t address;
	uint16_t size;
} __attribute__((packed));

struct gdt gdt;
struct gdt_descriptor gdtd;

/* Legacy Segment Descriptor structure:
*	bits 63-56: Base Address [31:24]
*	bits 55-52: Flags (G, D/B, L, AVL)
*	bits 51-48: Segment Limit [19:16]
*	bits 47-40: Access Byte (P, DPL(3), S, E, D/C, R/W, A)
*	bits 39-16: Base Address [23:0]
*	bits 15-0:  Segment Limit [15:0]
*/

/** encode_gdt_entry:
 * 	Encondes the GDT entry with the given values.
 * 
 * 	@param entry 		the GDT entry
 * 	@param base 		the entry's base address
 * 	@param limit		the entry's maximum address
 * 	@param access_byte 	the entry's access byte
 * 	@param flags 		the entry's flags
*/
static void encode_gdt_entry(uint8_t entry[8], uint32_t base, uint32_t limit, uint8_t access_byte, uint8_t flags) {
	// Although this function takes a 32 bit limit and 8 bit flags, only 20 bits and 4 bits, respectively, are used.

	// encode the base
	entry[2] = base & 0xFF;
	entry[3] = (base >> 8) & 0xFF;
	entry[4] = (base >> 16) & 0xFF;
	entry[7] = (base >> 24) & 0xFF;

	// encode the limit and flags
	entry[0] = limit & 0xFF;
	entry[1] = (limit >> 8) & 0xFF;
	entry[6] = (flags << 4) | ((limit >> 16) & 0x0F);

	// encode the access byte
	entry[5] = access_byte;
}

void init_gdt(void) {
	encode_gdt_entry(gdt.null_descriptor, 0x0, 0x0, 0x0, 0x0);
	encode_gdt_entry(gdt.kernel_mode_code_segment, FLAT_MODE_BASE, FLAT_MODE_LIMIT, KERNEL_MODE_CODE_SEGMENT_ACCESS_BYTE, LEGACY_MODE_SEGMENT_FLAGS);
	encode_gdt_entry(gdt.kernel_mode_data_segment, FLAT_MODE_BASE, FLAT_MODE_LIMIT, KERNEL_MODE_DATA_SEGMENT_ACCESS_BYTE, LEGACY_MODE_SEGMENT_FLAGS);
	encode_gdt_entry(gdt.user_mode_code_segment, FLAT_MODE_BASE, FLAT_MODE_LIMIT, USER_MODE_CODE_SEGMENT_ACCESS_BYTE, LEGACY_MODE_SEGMENT_FLAGS);
	encode_gdt_entry(gdt.user_mode_data_segment, FLAT_MODE_BASE, FLAT_MODE_LIMIT, USER_MODE_DATA_SEGMENT_ACCESS_BYTE, LEGACY_MODE_SEGMENT_FLAGS);
	encode_gdt_entry(gdt.task_state_segment, (uint32_t) &tss, sizeof(struct tss), TASK_STATE_SEGMENT_ACCESS_BYTE, 0x0);
}

void load_gdt(void) {
	gdtd.address = (uint32_t) &gdt;
	gdtd.size = (uint16_t) sizeof(struct gdt);
	asm volatile("lgdt [%0]" : : "r" (&gdtd));
}

