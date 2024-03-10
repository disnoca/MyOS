#pragma once

#include <stdint.h>

/**
 *  Sends a byte to an I/O port.
 *
 *  @param port the I/O port
 *  @param data the data to be sent
 */
static inline void outb(uint16_t port, uint8_t data)
{
	asm volatile("out dx, al" : : "a" (data), "d" (port));
}

/**
 *  Reads a byte from an I/O port.
 *
 *  @param port the I/O port
 * 
 *  @return the read byte
 */
static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile("in al, dx" : "=a" (data) : "d" (port));
	return data;
}

/**
 *  Sends a doubleword to an I/O port.
 *
 *  @param port the I/O port
 *  @param data the data to be sent
 */
static inline void outd(uint16_t port, uint32_t data)
{
	asm volatile("out dx, eax" : : "a" (data), "d" (port));
}

/**
 *  Reads a doubleword from an I/O port.
 *
 *  @param port the address of the I/O port
 * 
 *  @return the read doubleword
 */
static inline uint32_t ind(uint16_t port)
{
	uint32_t data;
	asm volatile("in eax, dx" : "=a" (data) : "d" (port));
	return data;
}
