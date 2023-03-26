#include "keyboard.h"
#include "../../arch/i386/io.h"
#include <kernel/tty.h>

#include <stdio.h>
#include <stdint.h>

#define KBD_DATA_PORT   						  0x60

#define SCAN_CODE_1 							  0x02
#define SCAN_CODE_2 							  0x03
#define SCAN_CODE_3 							  0x04
#define SCAN_CODE_4 							  0x05
#define SCAN_CODE_5 							  0x06
#define SCAN_CODE_6 							  0x07
#define SCAN_CODE_7 							  0x08
#define SCAN_CODE_8 							  0x09
#define SCAN_CODE_9 							  0x0A
#define SCAN_CODE_0 							  0x0B


void read_input_from_keyboard(void) {
    uint8_t scan_code = inb(KBD_DATA_PORT);
	
	if(scan_code >> 7) return;

	char c;

	switch (scan_code) {
	case(SCAN_CODE_1): c = '1'; break;
	case(SCAN_CODE_2): c = '2'; break;
	case(SCAN_CODE_3): c = '3'; break;
	case(SCAN_CODE_4): c = '4'; break;
	case(SCAN_CODE_5): c = '5'; break;
	case(SCAN_CODE_6): c = '6'; break;
	case(SCAN_CODE_7): c = '7'; break;
	case(SCAN_CODE_8): c = '8'; break;
	case(SCAN_CODE_9): c = '9'; break;
	case(SCAN_CODE_0): c = '0'; break;
	default: break;
	}

	tty_putchar(c);
}