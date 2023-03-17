/**
 * Code for setting up and using the Programmable Interrupt Controller
 * 
 * Refer to:
 * https://web.archive.org/web/20071012022705/http://www.acm.uiuc.edu/sigops/roll_your_own/i386/irq.html
 * https://wiki.osdev.org/8259
 * 
 * @author Samuel Pires
*/

#include "pic.h"
#include "io.h"

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

/* PIC Mask: Disables all interrupts whose bits are set to 1. */
void pic_set_mask(uint16_t mask) {
	outb((mask & 0xFF), MASTER_DATA_PORT);
	outb(((mask >> 8) & 0xFF), SLAVE_DATA_PORT);
}

void pic_init(void) {
	/* ICW1 */
	outb(0x11, MASTER_COMMAND_PORT); 		// start initialization sequence
	outb(0x11, SLAVE_COMMAND_PORT);

	/* ICW2 */
	outb(MASTER_OFFSET, MASTER_DATA_PORT); 	// set master offset in the IDT
	outb(SLAVE_OFFSET, SLAVE_DATA_PORT);	// set slave offset in the IDT

	/* ICW3 */
	outb(0x04, MASTER_DATA_PORT); 			// tell master that slave is at IRQ2
	outb(0x02, SLAVE_DATA_PORT); 			// tell slave it's in master's IRQ2

	/* ICW4 */
	outb(0x05, MASTER_DATA_PORT); 			// set as master
	outb(0x01, SLAVE_DATA_PORT); 			// set as slave

	pic_set_mask(0xFFF8);					// only listen to timer and keyboard by default
}

void pic_send_eoi(uint8_t irq) {
	// it's only necessary to send an eoi to the slave if the IRQ came from it
	if(irq >= 8)
		outb(SLAVE_COMMAND_PORT, EOI_CODE);
	
	outb(MASTER_COMMAND_PORT, EOI_CODE);
}