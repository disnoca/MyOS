#ifndef SERIAL_PORT
#define SERIAL_PORT

#define ERROR_LOG 0
#define WARNING_LOG 1
#define INFO_LOG 2

/** serial_init:
 *  Initializes the serial ports.
 * 
 *  @return 0 if all serial ports are working correctly
 *          1 if a serial port is faulty
*/
int serial_init();

/** log:
 *  Writes a log message. Assumes the message ends with a null character. 
 *  The null character is transformed into a new line when logging the message,
 *  by modifying the string itself.
 * 
 *  @param msg  the message to be written
 *  @param type the type of log message
*/
void log(char* msg, unsigned int type);

#endif