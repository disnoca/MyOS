#pragma once

#include <stddef.h>
#include <stdbool.h>


/**
 *  Initializes the serial ports.
 * 
 *  @return true if all serial ports are working correctly or false if a serial port is faulty
*/
int serial_init();

/**
 *  Writes size bytes from the given data to the serial port.
 * 
 *  @param data the data to be written
 * 	@param size the size of the data
*/
void serial_write(const char* data, size_t size);

/**
 *  Writes a null terminated string to the serial port.
 * 
 *  @param str the string to be written
*/
void serial_writestring(const char* str);

/**
 * 	Writes the given character to the serial port.
 * 
 * 	@param c the char to be written
*/
void serial_writechar(char c);
