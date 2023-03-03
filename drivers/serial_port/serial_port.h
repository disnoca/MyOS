/** serial_init:
 *  Initializes the serial ports.
 * 
 *  @return 0 if all serial ports are working correctly
 *          1 if a serial port is faulty
*/
int serial_init();

/** serial_write:
 *  Writes a character to the serial port.
 * 
 *  @param c the character to be written
*/
void serial_write(char* buf, unsigned int len);