#ifndef LAYOUTS
#define LAYOUTS

#include <stdint.h>

#define SCAN_CODE_A 			0x1E
#define SCAN_CODE_B 			0x30
#define SCAN_CODE_C 			0x2E
#define SCAN_CODE_D 			0x20
#define SCAN_CODE_E 			0x12
#define SCAN_CODE_F 			0x21
#define SCAN_CODE_G 			0x22
#define SCAN_CODE_H 			0x23
#define SCAN_CODE_I 			0x17
#define SCAN_CODE_J 			0x24
#define SCAN_CODE_K 			0x25
#define SCAN_CODE_L 			0x26
#define SCAN_CODE_M 			0x32
#define SCAN_CODE_N 			0x31
#define SCAN_CODE_O 			0x18
#define SCAN_CODE_P 			0x19
#define SCAN_CODE_Q 			0x10
#define SCAN_CODE_R 			0x13
#define SCAN_CODE_S 			0x1F
#define SCAN_CODE_T 			0x14
#define SCAN_CODE_U 			0x16
#define SCAN_CODE_V 			0x2F
#define SCAN_CODE_W 			0x11
#define SCAN_CODE_X 			0x2D
#define SCAN_CODE_Y 			0x15
#define SCAN_CODE_Z 			0x2C

#define SCAN_CODE_0 			0x0B
#define SCAN_CODE_1 			0x02
#define SCAN_CODE_2 			0x03
#define SCAN_CODE_3 			0x04
#define SCAN_CODE_4 			0x05
#define SCAN_CODE_5 			0x06
#define SCAN_CODE_6 			0x07
#define SCAN_CODE_7 			0x08
#define SCAN_CODE_8 			0x09
#define SCAN_CODE_9 			0x0A

#define SCAN_CODE_BACKTICK 		0x29
#define SCAN_CODE_MINUS 		0x0C
#define SCAN_CODE_EQUALS 		0x0D
#define SCAN_CODE_LEFT_BRACKET 	0x1A
#define SCAN_CODE_RIGHT_BRACKET 0x1B
#define SCAN_CODE_SEMICOLON 	0x27
#define SCAN_CODE_APOSTROPHE 	0x28
#define SCAN_CODE_HASH 			0x2B
#define SCAN_CODE_BACKSLASH 	0x56
#define SCAN_CODE_COMMA 		0x33
#define SCAN_CODE_PERIOD 		0x34
#define SCAN_CODE_SLASH 		0x35

#define SCAN_CODE_SPACE 		0x39
#define SCAN_CODE_ENTER 		0x1C
#define SCAN_CODE_BACKSPACE 	0x0E

#define SCAN_CODE_LSHIFT 		0x2A
#define SCAN_CODE_RSHIFT 		0x36
#define SCAN_CODE_CTRL 			0x1D
#define SCAN_CODE_ALT 			0x38
#define SCAN_CODE_CAPS_LOCK 	0x3A

char read_key_portuguese(uint8_t scan_code, uint8_t shift_pressed, uint8_t right_alt_pressed);

char read_key_american(uint8_t scan_code, uint8_t shift_pressed, uint8_t right_alt_pressed);

#endif