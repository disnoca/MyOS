#include "keyboard.h"
#include "layouts.h"
#include "../../arch/i386/io.h"
#include <kernel/tty.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define KBD_DATA_PORT   0x60

#define EXTENDED_KEY 	0xE0

#define SCANCODE_LSHIFT 0x2A
#define SCANCODE_RSHIFT 0x36
#define SCANCODE_CTRL 	0x1D
#define SCANCODE_ALT 	0x38
#define SCANCODE_CAPS 	0x3A

#define FLAG_LSHIFT 	0x01
#define FLAG_RSHIFT 	0x02
#define FLAG_LCTRL 		0x04
#define FLAG_RCTRL  	0x08
#define FLAG_LALT 		0x10
#define FLAG_RALT 		0x20
#define FLAG_CAPS 		0x40

#define FLAGS_SHIFT 	(FLAG_LSHIFT | FLAG_RSHIFT)
#define FLAGS_CTRL 		(FLAG_LCTRL | FLAG_RCTRL)
#define FLAGS_ALT 		(FLAG_LALT | FLAG_RALT)

static uint8_t special_keys_flags = 0;


#define IS_LOWERCASE_LETTER(c)			(c >= 'a' && c <= 'z')
#define LOWERCASE_TO_UPPERCASE(c)		(c - 'a' + 'A')


#define TABLE_NORMAL 0
#define TABLE_SHIFT  1
#define TABLE_ALTGR  2

static char (*scan_to_ascii)[SCAN_TO_ASCII_TABLE_SIZE] = scan_to_ascii_pt;


/**
 * 	Reads the received scan code and updates the special keys flags accordingly,
 * 	returning 1 if the scan code was a special key, 0 otherwise.
 * 
 * 	@param had_extended_byte whether the scan code had an extended byte or not
 * 	@param scan_code 		 the scan code's least significant byte
 * 	@return				 	 1 if the scan code was a special key, 0 otherwise
*/
static uint8_t read_special_key(uint8_t scan_code, bool had_extended_byte)
{
	switch (scan_code) {

	/* enable special key flag when key is pressed */
	case(SCANCODE_LSHIFT):
		special_keys_flags |= FLAG_LSHIFT;
		break;

	/* disable special key flag when key is released (scan_code's most significant bit is set) */
	case(SCANCODE_LSHIFT | 0x80):
		special_keys_flags &= ~FLAG_LSHIFT;
		break;

	case(SCANCODE_RSHIFT):
		special_keys_flags |= FLAG_RSHIFT;
		break;

	case(SCANCODE_RSHIFT | 0x80):
		special_keys_flags &= ~FLAG_RSHIFT;
		break;

	/* find out whether left or right key was pressed based on if the extended byte was received or not */
	case(SCANCODE_CTRL):
		special_keys_flags |= (had_extended_byte ? FLAG_RCTRL : FLAG_LCTRL);
		break;

	case(SCANCODE_CTRL | 0x80):
		special_keys_flags &= ~(had_extended_byte ? FLAG_RCTRL : FLAG_LCTRL);
		break;

	case(SCANCODE_ALT):
		special_keys_flags |= (had_extended_byte ? FLAG_RALT : FLAG_LALT);
		break;

	case(SCANCODE_ALT | 0x80):
		special_keys_flags &= ~(had_extended_byte ? FLAG_RALT : FLAG_LALT);
		break;
	
	/* use xor for toggling lock keys */
	case(SCANCODE_CAPS):
		special_keys_flags ^= FLAG_CAPS;
		break;

	default: return 0;
	}

	return 1;
}

void read_input_from_keyboard(void)
{
    uint8_t scan_code = inb(KBD_DATA_PORT);
	bool had_extended_byte = 0;

	if(scan_code == EXTENDED_KEY) {
		scan_code = inb(KBD_DATA_PORT);
		had_extended_byte = 1;
	}

	if(read_special_key(scan_code, had_extended_byte))
		return;

	/* return on (non-special) key released */
	if(scan_code >> 7)
		return;

	char c;

	if(special_keys_flags & FLAGS_SHIFT)
		c = scan_to_ascii[TABLE_SHIFT][scan_code];

	/* for some reason altgr is sending both alts instead of the expected alt+ctrl as scan
	   codes therefore we check if both alt flags are set to see if altgr is being pressed */
	else if((special_keys_flags & FLAGS_ALT) == FLAGS_ALT)
		c = scan_to_ascii[TABLE_ALTGR][scan_code];

	else {
		c = scan_to_ascii[TABLE_NORMAL][scan_code];

		if((special_keys_flags & FLAG_CAPS) && IS_LOWERCASE_LETTER(c))
			c = LOWERCASE_TO_UPPERCASE(c);
	}

	/* invalid/not yet implemented character */
	if(c == 0)
		return;

	tty_putchar(c);
}
