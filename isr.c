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
	switch (vector_id) {
		case(0): fb_write("0 "); break;
		case(1): fb_write("1 "); break;
		case(2): fb_write("2 "); break;
		case(3): fb_write("3 "); break;
		case(4): fb_write("4 "); break;
		case(5): fb_write("5 "); break;
		case(6): fb_write("6 "); break;
		case(7): fb_write("7 "); break;
		case(8): fb_write("8 "); break;
		case(9): fb_write("9 "); break;
		case(10): fb_write("10 "); break;
		case(11): fb_write("11 "); break;
		case(12): fb_write("12 "); break;
		case(13): fb_write("13 "); break;
		case(14): fb_write("14 "); break;
		case(15): fb_write("15 "); break;
		case(16): fb_write("16 "); break;
		case(17): fb_write("17 "); break;
		case(18): fb_write("18 "); break;
		case(19): fb_write("19 "); break;
		case(20): fb_write("20 "); break;
		case(21): fb_write("21 "); break;
		case(22): fb_write("22 "); break;
		case(23): fb_write("23 "); break;
		case(24): fb_write("24 "); break;
		case(25): fb_write("25 "); break;
		case(26): fb_write("26 "); break;
		case(27): fb_write("27 "); break;
		case(28): fb_write("28 "); break;
		case(29): fb_write("29 "); break;
		case(30): fb_write("30 "); break;
		case(31): fb_write("31 "); break;
		default: fb_write("none "); break;
	}
}