/**	pic_init:
 * 	Initializes the PIC.
*/
void pic_init(void);

/**	pic_set_mask:
 * 	Sets the mask for the PIC.
 * 
 * 	@param mask the mask to be set
*/
void pic_set_mask(uint16_t mask);

/**	pic_send_eoi:
 * 	Sends a EOI to the PIC.
 * 
 * 	@param irq the IRQ that was handled
*/
void pic_send_eoi(uint8_t irq);