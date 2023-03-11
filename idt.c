/**
 * Code for setting up the Interrupt Descriptor Table along with the Interrupt Service Routines.
 * 
 * Refer to:
 * AMD64 Architecture Programmer's Manual Volume 2: System Programming Section 8
 * https://wiki.osdev.org/Interrupt_Descriptor_Table
 * 
 * @author Samuel Pires
*/

#include "idt.h"
#include "isr.h"
#include "types.h"

#define HARDWARE_INTERRUPT_ENTRIES   32
#define MAX_INTERRUPT_ENTRIES        256

#define KERNEL_CODE_SEGMENT_SELECTOR 0x8
#define HARDWARE_INTERRUPT_FLAGS     0x8
#define BIT32_INTERRUPT_GATE         0xE

/* Interrupt/Trap Gate Descriptor
 *   bits 63-48: offset [31:16]
 *   bits 47-44: flags
 *   bits 43-40: gate type
 *   bits 39-32: reserved
 *   bits 31-16: segment selector
 *   bits 15-0:  offset [15:0]
*/
struct igd {
	uint16_t offset_lb;
	uint16_t segment_selector;
	uint8_t reserved;
	uint8_t flags_and_type;
	uint16_t offset_ub;
} __attribute__((packed));

static struct igd idt[HARDWARE_INTERRUPT_ENTRIES];

static void encode_igd(unsigned int vector_id, uint32_t handler, uint8_t flags, uint8_t type) {
	// only 4 bits of both the flags and type are used

	// encode the offset
	idt[vector_id].offset_lb = (uint16_t) handler & 0xFFFF;
	idt[vector_id].offset_ub = (uint16_t) (handler >> 16) & 0xFFFF;

	// encode the segment
	idt[vector_id].segment_selector = (uint16_t) KERNEL_CODE_SEGMENT_SELECTOR;

	// encode the flags and type
	idt[vector_id].flags_and_type = (uint8_t) ((flags << 4) | (type & 0x0F));
}

void idt_init(void) {
	encode_igd(0, (uint32_t) interrupt_handler_0, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(1, (uint32_t) interrupt_handler_1, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(2, (uint32_t) interrupt_handler_2, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(3, (uint32_t) interrupt_handler_3, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(4, (uint32_t) interrupt_handler_4, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(5, (uint32_t) interrupt_handler_5, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(6, (uint32_t) interrupt_handler_6, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(7, (uint32_t) interrupt_handler_7, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(8, (uint32_t) interrupt_handler_8, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(9, (uint32_t) interrupt_handler_9, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(10, (uint32_t) interrupt_handler_10, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(11, (uint32_t) interrupt_handler_11, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(12, (uint32_t) interrupt_handler_12, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(13, (uint32_t) interrupt_handler_13, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(14, (uint32_t) interrupt_handler_14, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(15, (uint32_t) interrupt_handler_15, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(16, (uint32_t) interrupt_handler_16, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(17, (uint32_t) interrupt_handler_17, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(18, (uint32_t) interrupt_handler_18, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(19, (uint32_t) interrupt_handler_19, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(20, (uint32_t) interrupt_handler_20, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(21, (uint32_t) interrupt_handler_21, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(22, (uint32_t) interrupt_handler_22, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(23, (uint32_t) interrupt_handler_23, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(24, (uint32_t) interrupt_handler_24, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(25, (uint32_t) interrupt_handler_25, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(26, (uint32_t) interrupt_handler_26, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(27, (uint32_t) interrupt_handler_27, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(28, (uint32_t) interrupt_handler_28, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(29, (uint32_t) interrupt_handler_29, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(30, (uint32_t) interrupt_handler_30, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
	encode_igd(31, (uint32_t) interrupt_handler_31, HARDWARE_INTERRUPT_FLAGS, BIT32_INTERRUPT_GATE);
}


/* Interrupt Descriptor Table Pseudo-Descriptor 
 *  bits 36-16: the Interrupt Descriptor Table's base address
 *  bits 15-0:  the Interrupt Descriptor Table's size
*/
static uint16_t idtd[3];

void idt_load(void) {
	idtd[2] = (uint16_t) (((uint32_t) &idt >> 16) & 0xFFFF);
	idtd[1] = (uint16_t) ((uint32_t) &idt & 0xFFFF);
	idtd[0] = (uint16_t) sizeof(idt);

	asm volatile("lidt [%0]" : : "r" (&idtd));
}

void isr_handler(/*uint32_t regs[8], uint32_t vector_id, uint32_t error_code*/) {
	/* do something */
}