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

void isr_handler(/*uint32_t regs[8], uint32_t vector_id, uint32_t error_code*/) {
	/* do something */
}