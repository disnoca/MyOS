#include "keyboard.h"
#include "../../arch/i386/io.h"
#include <kernel/tty.h>

#include <stdio.h>
#include <stdint.h>

#define KBD_DATA_PORT   	0x60

#define EXTENDED_KEY 		0xE0


#define SCAN_CODE_A 		0x1E
#define SCAN_CODE_B 		0x30
#define SCAN_CODE_C 		0x2E
#define SCAN_CODE_D 		0x20
#define SCAN_CODE_E 		0x12
#define SCAN_CODE_F 		0x21
#define SCAN_CODE_G 		0x22
#define SCAN_CODE_H 		0x23
#define SCAN_CODE_I 		0x17
#define SCAN_CODE_J 		0x24
#define SCAN_CODE_K 		0x25
#define SCAN_CODE_L 		0x26
#define SCAN_CODE_M 		0x32
#define SCAN_CODE_N 		0x31
#define SCAN_CODE_O 		0x18
#define SCAN_CODE_P 		0x19
#define SCAN_CODE_Q 		0x10
#define SCAN_CODE_R 		0x13
#define SCAN_CODE_S 		0x1F
#define SCAN_CODE_T 		0x14
#define SCAN_CODE_U 		0x16
#define SCAN_CODE_V 		0x2F
#define SCAN_CODE_W 		0x11
#define SCAN_CODE_X 		0x2D
#define SCAN_CODE_Y 		0x15
#define SCAN_CODE_Z 		0x2C

#define SCAN_CODE_0 		0x0B
#define SCAN_CODE_1 		0x02
#define SCAN_CODE_2 		0x03
#define SCAN_CODE_3 		0x04
#define SCAN_CODE_4 		0x05
#define SCAN_CODE_5 		0x06
#define SCAN_CODE_6 		0x07
#define SCAN_CODE_7 		0x08
#define SCAN_CODE_8 		0x09
#define SCAN_CODE_9 		0x0A

#define SCAN_CODE_SPACE 	0x39
#define SCAN_CODE_ENTER 	0x1C
#define SCAN_CODE_BACKSPACE 0x0E

#define SCAN_CODE_LSHIFT 	0x2A
#define SCAN_CODE_RSHIFT 	0x36
#define SCAN_CODE_CTRL 		0x1D
#define SCAN_CODE_ALT 		0x38
#define SCAN_CODE_CAPS_LOCK 0x3A


#define LEFT_SHIFT_FLAG 	0x01
#define RIGHT_SHIFT_FLAG 	0x02
#define LEFT_CTRL_FLAG 		0x04
#define RIGHT_CTRL_FLAG 	0x08
#define LEFT_ALT_FLAG 		0x10
#define RIGHT_ALT_FLAG 		0x20
#define CAPS_LOCK_FLAG 		0x40
uint8_t special_keys_flags = 0;

#define SHIFT_MASK 		(LEFT_SHIFT_FLAG | RIGHT_SHIFT_FLAG)
#define CTRL_MASK 		(LEFT_CTRL_FLAG | RIGHT_CTRL_FLAG)
#define ALT_MASK 		(LEFT_ALT_FLAG | RIGHT_ALT_FLAG)
#define USE_CAPS_MASK 	(SHIFT_MASK | CAPS_LOCK_FLAG)


#define UPPERCASE_TO_LOWERCASE_DIFF ('a' - 'A')
#define IS_LOWERCASE_LETTER(c)		(c >= 'a' && c <= 'z')


/**	read_special_key:
 * 	Reads the received scan code and updates the special keys flags accordingly,
 * 	returning 1 if the scan code was a special key, 0 otherwise.
 * 
 * 	@param had_extended_byte whether the scan code had an extended byte or not
 * 	@param scan_code 		 the scan code's least significant byte
 * 	@return				 	 1 if the scan code was a special key, 0 otherwise
*/
static uint8_t read_special_key(uint8_t had_extended_byte, uint8_t scan_code) {
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

/**	read_key:
 * 	Reads the received scan code and returns the corresponding character.
 * 
 * 	@param scan_code the scan code
 * 	@return			 the corresponding character
*/
static char read_key(uint8_t scan_code) {
	switch(scan_code) {
	case(SCAN_CODE_A): return 'a';
	case(SCAN_CODE_B): return 'b';
	case(SCAN_CODE_C): return 'c';
	case(SCAN_CODE_D): return 'd';
	case(SCAN_CODE_E): return 'e';
	case(SCAN_CODE_F): return 'f';
	case(SCAN_CODE_G): return 'g';
	case(SCAN_CODE_H): return 'h';
	case(SCAN_CODE_I): return 'i';
	case(SCAN_CODE_J): return 'j';
	case(SCAN_CODE_K): return 'k';
	case(SCAN_CODE_L): return 'l';
	case(SCAN_CODE_M): return 'm';
	case(SCAN_CODE_N): return 'n';
	case(SCAN_CODE_O): return 'o';
	case(SCAN_CODE_P): return 'p';
	case(SCAN_CODE_Q): return 'q';
	case(SCAN_CODE_R): return 'r';
	case(SCAN_CODE_S): return 's';
	case(SCAN_CODE_T): return 't';
	case(SCAN_CODE_U): return 'u';
	case(SCAN_CODE_V): return 'v';
	case(SCAN_CODE_W): return 'w';
	case(SCAN_CODE_X): return 'x';
	case(SCAN_CODE_Y): return 'y';
	case(SCAN_CODE_Z): return 'z';

	case(SCAN_CODE_SPACE): return ' ';
	case(SCAN_CODE_ENTER): return '\n';
	case(SCAN_CODE_BACKSPACE): return '\b';

	case(SCAN_CODE_1): return '1';
	case(SCAN_CODE_2): return '2';
	case(SCAN_CODE_3): return '3';
	case(SCAN_CODE_4): return '4';
	case(SCAN_CODE_5): return '5';
	case(SCAN_CODE_6): return '6';
	case(SCAN_CODE_7): return '7';
	case(SCAN_CODE_8): return '8';
	case(SCAN_CODE_9): return '9';
	case(SCAN_CODE_0): return '0';

	default: return 0;
	}
}

void read_input_from_keyboard(void) {
    uint8_t scan_code = inb(KBD_DATA_PORT);
	uint8_t had_extended_byte = 0;

	if(scan_code == EXTENDED_KEY) {
		scan_code = inb(KBD_DATA_PORT);
		had_extended_byte = 1;
	}

	if(read_special_key(had_extended_byte, scan_code))
		return;

	char c = read_key(scan_code);

	/* invalid/not yet implemented character */
	if(c == 0)
		return;

	if(IS_LOWERCASE_LETTER(c) && (special_keys_flags & USE_CAPS_MASK))
		c -= UPPERCASE_TO_LOWERCASE_DIFF;

	tty_putchar(c);
}