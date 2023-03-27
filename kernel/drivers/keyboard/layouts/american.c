#include "layouts.h"

char read_key_american(uint8_t scan_code, bool shift_pressed, __attribute__((unused)) bool altgr_pressed) {

	if(shift_pressed)
		switch(scan_code) {
		case(SCAN_CODE_BACKTICK): return '~';
		case(SCAN_CODE_1): return '!';
		case(SCAN_CODE_2): return '"';
		//case(SCAN_CODE_3): return '£';
		case(SCAN_CODE_4): return '$';
		case(SCAN_CODE_5): return '%';
		case(SCAN_CODE_6): return '^';
		case(SCAN_CODE_7): return '&';
		case(SCAN_CODE_8): return '*';
		case(SCAN_CODE_9): return '(';
		case(SCAN_CODE_0): return ')';
		case(SCAN_CODE_MINUS): return '_';
		case(SCAN_CODE_EQUALS): return '+';
		case(SCAN_CODE_LEFT_BRACKET): return '{';
		case(SCAN_CODE_RIGHT_BRACKET): return '}';
		case(SCAN_CODE_SEMICOLON): return ':';
		case(SCAN_CODE_APOSTROPHE): return '@';
		case(SCAN_CODE_HASH): return '~';
		case(SCAN_CODE_BACKSLASH): return '|';
		case(SCAN_CODE_COMMA): return '<';
		case(SCAN_CODE_PERIOD): return '>';
		case(SCAN_CODE_SLASH): return '?';
		default: break;
		}
	
	switch(scan_code) {
	case(SCAN_CODE_BACKTICK): return '`';
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
	case(SCAN_CODE_MINUS): return '-';
	case(SCAN_CODE_EQUALS): return '=';
	case(SCAN_CODE_LEFT_BRACKET): return '[';
	case(SCAN_CODE_RIGHT_BRACKET): return ']';
	case(SCAN_CODE_SEMICOLON): return ';';
	case(SCAN_CODE_APOSTROPHE): return '\'';
	case(SCAN_CODE_HASH): return '#';
	//case(SCAN_CODE_BACKSLASH): return '\\';
	case(SCAN_CODE_COMMA): return ',';
	case(SCAN_CODE_PERIOD): return '.';
	case(SCAN_CODE_SLASH): return '/';

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
	case(0x56): return 'a';
	default: return 0;
	}
}