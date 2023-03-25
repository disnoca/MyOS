/**
 * Code for handling all the different Interrupt Service Routines.
 * 
 * Complement to isr_entry.S which serves as an entry point to and passes all ISRs 
 * through a common function, uniformising them before redirecting them here.
 * 
 * Refer to:
 * AMD64 Architecture Programmer's Manual Volume 2: System Programming Section 8
 * 
 * @author Samuel Pires
*/

#include <stdint.h>
#include <kernel/pic.h>
#include "../../drivers/keyboard.h"

#define PIC_OFFSET 32


struct isr_frame {
	uint32_t regs[8];
	uint32_t vector_id;
	uint32_t error_code;
};


void isr_handler(struct isr_frame isr_frame) {
	switch(isr_frame.vector_id) {
		case(0): break;
		case(1): break;
		case(2): break;
		case(3): break;
		case(4): break;
		case(5): break;
		case(6): break;
		case(7): break;
		case(8): break;
		case(9): break;
		case(10): break;
		case(11): break;
		case(12): break;
		case(13): break;
		case(14): break;
		case(15): break;
		case(16): break;
		case(17): break;
		case(18): break;
		case(19): break;
		case(20): break;
		case(21): break;
		case(22): break;
		case(23): break;
		case(24): break;
		case(25): break;
		case(26): break;
		case(27): break;
		case(28): break;
		case(29): break;
		case(30): break;
		case(31): break;

		case(32): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(33): read_input_from_keyboard(); pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(34): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(35): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(36): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(37): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(38): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(39): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(40): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(41): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(42): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(43): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(44): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(45): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(46): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;
		case(47): pic_send_eoi(isr_frame.vector_id - PIC_OFFSET); break;

		default: break;
	}
}