#ifndef PARSE
#define PARSE

/**	parse_unsigned:
 * 	Parses an unsigned integer into a string and stores it in the given buffer.
 * 	
 * @param buf the buffer to store the parsed string on
 * @param val the unsigned integer to be parsed
*/
void parse_unsigned(char* buf, unsigned int val);

/**	parse_hexadecimal:
 * 	Parses an hexadecimal number into a string and stores it in the given buffer.
 * 	
 * @param buf the buffer to store the parsed string on
 * @param val the hexadecimal number to be parsed
*/
void parse_hexadecimal(char* buf, unsigned int val);

#endif