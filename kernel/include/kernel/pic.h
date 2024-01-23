#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

#include <stdint.h>

/**
 * 	Initializes the PIC.
*/
void pic_init(void);

/**
 * 	Sets the mask for the PIC.
 * 
 * 	@param mask the mask to be set
*/
void pic_set_mask(uint16_t mask);

/**
 * 	Sends a EOI to the PIC.
 * 
 * 	@param irq the IRQ that was handled
*/
void pic_send_eoi(uint8_t irq);

#endif
