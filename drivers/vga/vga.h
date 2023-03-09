#ifndef VGA
#define VGA

#include "../../types.h"

/** enable_cursor:
 *  Enables the cursor and sets the start and end scanlines
 * 
 * @param cursor_start highest (minimum) scanline
 * @param cursor_end   lowest (maximum) scanline
*/
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

/** disable_cursor:
 *  Disables the cursor
*/
void disable_cursor(void);

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos new position of the cursor
*/
void move_cursor(uint16_t pos);

/** fb_init_cursor:
 *  Enables the cursor, setting its scanlines to their respective maximums 
 *  and sets its position to 0
*/
void init_cursor(void);

/** fb_write:
 *  Writes the given characters to the framebuffer
 * 
 *  @param buf characters to be written
 *  @param len number of characters to be written
*/
void fb_write(char* buf, size_t len);

#endif