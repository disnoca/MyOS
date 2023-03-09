#ifndef GDT
#define GDT

#include "types.h"

struct gdtr {
	uint32_t address;
	uint16_t size;
} __attribute__((packed));

struct gdt {
	uint8_t null_descriptor[8];
	uint8_t kernel_mode_code_segment[8];
	uint8_t kernel_mode_data_segment[8];
	uint8_t user_mode_code_segment[8];
	uint8_t user_mode_data_segment[8];
	uint8_t task_state_segment[8];
} __attribute__((packed));

extern struct gdt gdt;

/**	init_gdt:
 * 	Encodes the Global Descriptor Table for Flat-Memory Mode (no segmentation)
*/
void init_gdt(void);

/*static inline void load_gdt(struct gdt gdt) {

}*/

#endif