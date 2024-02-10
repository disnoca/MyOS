#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

/**
 * Encodes the Interrupt Descriptor Table with the ISR Handlers, loads the Interrupt Descriptor Table
 * into the Interrupt Descriptor Table Register and enables interrupts.
*/
void idt_init(void);

#endif
