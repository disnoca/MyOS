/**	idt_init:
 * 	Encodes the Interrupt Descriptor Table with the ISR Handlers.
*/
void idt_init(void);

/**	idt_load:
 * 	Loads the Interrupt Descriptor Table into the Interrupt Descriptor Table Register
 * 	and enables interrupts.
*/
void idt_load(void);