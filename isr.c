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

#include "types.h"
#include "libs/parse.h"
#include "drivers/vga/vga.h"

// it will have to stay like this until I find out why an array doesn't work or change it to a struct
void isr_handler(__attribute__((unused)) uint32_t a, __attribute__((unused)) uint32_t b, __attribute__((unused)) uint32_t c, __attribute__((unused)) uint32_t d, __attribute__((unused)) uint32_t e, __attribute__((unused)) uint32_t f, __attribute__((unused)) uint32_t g, __attribute__((unused)) uint32_t h, uint32_t vector_id, __attribute__((unused)) uint32_t error_code) {
	switch(vector_id) {
		case(33): fb_write("keyboard pressed\n"); break;
		default: break;
	}
}