/**
 * Code for setting up and using the Programmable Interrupt Controller
 * 
 * Refer to:
 * https://web.archive.org/web/20071012022705/http://www.acm.uiuc.edu/sigops/roll_your_own/i386/irq.html
 * https://wiki.osdev.org/8259
 * 
 * @author Samuel Pires
*/

#include <kernel/arch/i386/pic.h>
#include <kernel/arch/i386/io.h>

#define MASTER_PORT			0x20
#define SLAVE_PORT			0xA0
#define MASTER_COMMAND_PORT	MASTER_PORT
#define MASTER_DATA_PORT	(MASTER_PORT+1)
#define SLAVE_COMMAND_PORT	SLAVE_PORT
#define SLAVE_DATA_PORT		(SLAVE_PORT+1)

/* Offsets into the IDT for interupt service routines */
#define MASTER_OFFSET		0x20
#define SLAVE_OFFSET		0x28

#define EOI_CODE			0x20		/* End-of-interrupt command code */

/* List of Interrupts:
 * 	IRQ0: 		Timer
 * 	IRQ1: 		Keyboard
 * 	IRQ2: 		Slave
 * 	IRQ3: 		COM2
 * 	IRQ4: 		COM1
 * 	IRQ5: 		LPT2
 * 	IRQ6: 		Floppy Disk
 * 	IRQ7: 		LPT1
 * 	IRQ8: 		Real Time Clock
 * 	IRQ9-IRQ12: General IO
 * 	IRQ13: 		Coprocessor
 * 	IRQ14-15: 	IDE Bus
*/

void pic_init(void)
{
	/* ICW1 */
	outb(MASTER_COMMAND_PORT, 0x11); 		// start initialization sequence
	outb(SLAVE_COMMAND_PORT, 0x11);

	/* ICW2 */
	outb(MASTER_DATA_PORT, MASTER_OFFSET); 	// set master offset in the IDT
	outb(SLAVE_DATA_PORT, SLAVE_OFFSET);	// set slave offset in the IDT

	/* ICW3 */
	outb(MASTER_DATA_PORT, 0x04); 			// tell master that slave is at IRQ2
	outb(SLAVE_DATA_PORT, 0x02); 			// tell slave it's in master's IRQ2

	/* ICW4 */
	outb(MASTER_DATA_PORT, 0x05); 			// set as master
	outb(SLAVE_DATA_PORT, 0x01); 			// set as slave

	pic_set_mask(0xFFFD);					// only listen to keyboard by default
}

/* PIC Mask: Disables all interrupts whose bits are set to 1. */
void pic_set_mask(uint16_t mask)
{
	outb(MASTER_DATA_PORT, (mask & 0xFF));
	outb(SLAVE_DATA_PORT, ((mask >> 8) & 0xFF));
}

void pic_send_eoi(uint8_t irq)
{
	// it's only necessary to send an eoi to the slave if the IRQ came from it
	if(irq >= 8)
		outb(SLAVE_COMMAND_PORT, EOI_CODE);
	
	outb(MASTER_COMMAND_PORT, EOI_CODE);
}
