#include "types.h"

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.S
 *
 *  @param port I/O port to send the data to
 *  @param data data to send to the I/O port
 */
void outb(uint16_t port, uint8_t data);

/** inb:
 *  Read a byte from an I/O port.
 *
 *  @param  port address of the I/O port
 *  @return      read byte
 */
uint8_t inb(uint16_t port);