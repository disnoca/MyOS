#include "parse.h"

#define START_32BIT_PARSE_VALUE 1000000000

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