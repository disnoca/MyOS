#ifndef IO
#define IO

#include "types.h"

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.S
 *
 *  @param port I/O port to send the data to
 *  @param data data to send to the I/O port
 */
static inline void outb(uint16_t port, uint8_t data) {
	asm volatile("out dx, al" : : "a" (data), "d" (port));
}

/** inb:
 *  Read a byte from an I/O port.
 *
 *  @param  port address of the I/O port
 *  @return      read byte
 */
static inline uint8_t inb(uint16_t port) {
	uint8_t data;
	asm volatile("in al, dx" : "=a" (data) : "d" (port));
	return data;
}

#endif