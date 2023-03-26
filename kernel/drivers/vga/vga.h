#ifndef VGA
#define VGA

#include <stdint.h>
#include <stddef.h>

/**	vga_init:
 * 	Clears the framebuffer and configures and enables the cursor.
*/
void vga_init(void);

/** cursor_enable:
 *  Enables the cursor.
*/
void cursor_enable(void);

/** cursor_disable:
 *  Disables the cursor.
*/
void cursor_disable(void);

/** fb_clear:
 * 	Clears the framebuffer and resets the cursor's position.
*/
void fb_clear(void);

/** fb_write:
 *  Writes size bytes from the given data to the framebuffer.
 * 
 *  @param data the data to be written
 * 	@param size the size of the data
*/
void fb_write(const char* data, size_t size);

/** fb_write:
 *  Writes a null terminated string to the framebuffer.
 * 
 *  @param str the string to be written
*/
void fb_writestring(const char* data);

/**	fb_writechar:
 * 	Writes the given character to the framebuffer.
 * 
 * 	@param c the char to be written
*/
void fb_writechar(char c);

#endif