#include "keyboard.h"
#include "layouts/layouts.h"
#include "../../arch/i386/io.h"
#include <kernel/tty.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define KBD_DATA_PORT    0x60

#define EXTENDED_KEY 	 0xE0

#define LEFT_SHIFT_FLAG  0x01
#define RIGHT_SHIFT_FLAG 0x02
#define LEFT_CTRL_FLAG 	 0x04
#define RIGHT_CTRL_FLAG  0x08
#define LEFT_ALT_FLAG 	 0x10
#define RIGHT_ALT_FLAG 	 0x20
#define CAPS_LOCK_FLAG 	 0x40
uint8_t special_keys_flags = 0;

#define SHIFT_FLAGS 		 (LEFT_SHIFT_FLAG | RIGHT_SHIFT_FLAG)
#define CTRL_FLAGS 		 (LEFT_CTRL_FLAG | RIGHT_CTRL_FLAG)
#define ALT_FLAGS 		 (LEFT_ALT_FLAG | RIGHT_ALT_FLAG)
#define USE_CAPS_FLAGS 	 (SHIFT_FLAGS | CAPS_LOCK_FLAG)


#define UPPERCASE_TO_LOWERCASE_DIFF ('a' - 'A')
#define IS_LOWERCASE_LETTER(c)		(c >= 'a' && c <= 'z')

/**	read_key:
 * 	Reads the received scan code and returns the corresponding character.
 * 	Returned letters are always in lowercase.
 * 
 * 	@param scan_code 	 the scan code
 * 	@param shift_pressed whether the shift key is being pressed or not
 * 	@param altgr_pressed whether the altgr key is being pressed or not
 * 	@return			 	 the corresponding character
*/
char (*read_key)(uint8_t, bool, bool) = read_key_portuguese;


/**	read_special_key:
 * 	Reads the received scan code and updates the special keys flags accordingly,
 * 	returning 1 if the scan code was a special key, 0 otherwise.
 * 
 * 	@param had_extended_byte whether the scan code had an extended byte or not
 * 	@param scan_code 		 the scan code's least significant byte
 * 	@return				 	 1 if the scan code was a special key, 0 otherwise
*/
static uint8_t read_special_key(uint8_t scan_code, bool had_extended_byte) {
	switch (scan_code) {

	/* enable special key flag when key is pressed */
	case(SCAN_CODE_LSHIFT):
		special_keys_flags |= LEFT_SHIFT_FLAG;
		break;

	/* disable special key flag when key is released (scan_code's most significant bit is set) */
	case(SCAN_CODE_LSHIFT | 0x80):
		special_keys_flags &= ~LEFT_SHIFT_FLAG;
		break;

	case(SCAN_CODE_RSHIFT):
		special_keys_flags |= RIGHT_SHIFT_FLAG;
		break;

	case(SCAN_CODE_RSHIFT | 0x80):
		special_keys_flags &= ~RIGHT_SHIFT_FLAG;
		break;

	/* find out whether left or right key was pressed based on if the extended byte was received or not */
	case(SCAN_CODE_CTRL):
		special_keys_flags |= (had_extended_byte ? RIGHT_CTRL_FLAG : LEFT_CTRL_FLAG);
		break;

	case(SCAN_CODE_CTRL | 0x80):
		special_keys_flags &= ~(had_extended_byte ? RIGHT_CTRL_FLAG : LEFT_CTRL_FLAG);
		break;

	case(SCAN_CODE_ALT):
		special_keys_flags |= (had_extended_byte ? RIGHT_ALT_FLAG : LEFT_ALT_FLAG);
		break;

	case(SCAN_CODE_ALT | 0x80):
		special_keys_flags &= ~(had_extended_byte ? RIGHT_ALT_FLAG : LEFT_ALT_FLAG);
		break;
	
	/* use xor for toggling lock keys */
	case(SCAN_CODE_CAPS_LOCK):
		special_keys_flags ^= CAPS_LOCK_FLAG;
		break;

	default: return 0;
	}

	return 1;
}

void read_input_from_keyboard(void) {
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

	char c = read_key(scan_code,
					  /* shift is being pressed */
					  (special_keys_flags & SHIFT_FLAGS),
					  /* for some reason altgr is sending both alts instead of the expected alt+ctrl as scan
	  					 codes therefore we check if both alt flags are set to see if altgr is being pressed */
					  ((special_keys_flags & ALT_FLAGS) == ALT_FLAGS));

	/* invalid/not yet implemented character */
	if(c == 0)
		return;

	/* returned char is a (always lowercase) letter and caps or shift are active */
	if((c >= 'a' && c <= 'z') && (special_keys_flags & USE_CAPS_FLAGS))
		c -= UPPERCASE_TO_LOWERCASE_DIFF;

	tty_putchar(c);
}