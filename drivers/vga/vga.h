#ifndef VGA
#define VGA

#include "../../types.h"

/** cursor_enable:
 *  Enables the cursor and sets the start and end scanlines
 * 
 * @param cursor_start highest (minimum) scanline
 * @param cursor_end   lowest (maximum) scanline
*/
void cursor_enable(uint8_t cursor_start, uint8_t cursor_end);

/** cursor_disable:
 *  Disables the cursor
*/
void cursor_disable(void);

/** cursor_move:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos new position of the cursor
*/
void cursor_move(uint16_t pos);

/** cursor_init:
 *  Enables the cursor, setting its scanlines to their respective maximums 
 *  and sets its position to 0
*/
void cursor_init(void);

/** fb_write:
 *  Writes the given characters to the framebuffer
 * 
 *  @param buf characters to be written
 *  @param len number of characters to be written
*/
void fb_write(char* buf, size_t len);

#endif