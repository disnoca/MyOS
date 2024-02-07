#ifndef _KERNEL_SERIAL_H
#define _KERNEL_SERIAL_H

#define LOG_ERROR 	0
#define LOG_WARNING 1
#define LOG_INFO 	2

/**
 *  Initializes the serial ports.
 * 
 *  @return 0 if all serial ports are working correctly or 1 if a serial port is faulty
*/
int serial_init();

/**
 *  Writes a string to the serial port.
 * 
 *  @param buf the string to be written
*/
void serial_write(char* buf);

#endif
