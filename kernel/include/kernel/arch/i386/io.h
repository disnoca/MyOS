#pragma once

#include <stdint.h>

/**
 *  Sends the given data to the given I/O port.
 *
 *  @param port the I/O port to send the data to
 *  @param data the data to be sent
 */
static inline void outb(uint16_t port, uint8_t data)
{
	asm volatile("out dx, al" : : "a" (data), "d" (port));
}

/**
 *  Reads a byte from an I/O port.
 *
 *  @param port the address of the I/O port
 *  @return the read byte
 */
static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile("in al, dx" : "=a" (data) : "d" (port));
	return data;
}
