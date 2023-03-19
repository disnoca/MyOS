#include "parse.h"
#include "../types.h"

#define START_32BIT_PARSE_VALUE     1000000000

#define BIT32_BYTE_COUNT            32 / 8
#define BIT32_HEX_CHAR_COUNT        32 / 4
#define NUMBER_TO_LETTER_ASCII_DIFF 'A' - '9' - 1

void parse_unsigned(char* buf, unsigned int val) {
    unsigned divisor = START_32BIT_PARSE_VALUE;
    unsigned curr_pos = 0;
    
    for(; val/divisor == 0 && divisor > 1; divisor /= 10) {}

	for(; divisor > 0; divisor /= 10) {
        unsigned curr_digit = val/divisor;
        buf[curr_pos++] = '0' + curr_digit;
        val -= curr_digit*divisor;
    }
}

static void parse_hex_byte(char buf[2], uint8_t byte) {
    uint8_t byte_lsb = byte & 0xF;
    uint8_t byte_msb = (byte >> 4) & 0xF;

    buf[1] = '0' + byte_lsb;
    buf[0] = '0' + byte_msb;

    if(byte_lsb >= 10)
        buf[1] += NUMBER_TO_LETTER_ASCII_DIFF;
    if(byte_msb >= 10)
        buf[0] += NUMBER_TO_LETTER_ASCII_DIFF;
}

void parse_hexadecimal(char* buf, uint32_t val) {
    buf[0] = '0'; buf[1] = 'x';
    buf += 2;

    uint8_t* curr_byte = (uint8_t*) &val;

    for(unsigned i = 0; i < BIT32_BYTE_COUNT; i++) {
        char hex_byte_chars[2];
        parse_hex_byte(hex_byte_chars, curr_byte[i]);

        buf[BIT32_HEX_CHAR_COUNT - i*2 - 1] = hex_byte_chars[1];
        buf[BIT32_HEX_CHAR_COUNT - i*2 - 2] = hex_byte_chars[0];
    }
}