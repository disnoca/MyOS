#pragma once

#include <stdint.h>
#include <stddef.h>

#define vga_clear() 			fb_clear()
#define vga_write(data, size) 	fb_write(data, size)
#define vga_writestring(str) 	fb_writestring(str)
#define vga_writechar(c) 		fb_writechar(c)

/**
 * 	Clears the framebuffer and configures and enables the cursor.
*/
void vga_init(void);

/**
 *  Enables the cursor.
*/
void cursor_enable(void);

/**
 *  Disables the cursor.
*/
void cursor_disable(void);

/**
 * 	Clears the framebuffer and resets the cursor's position.
*/
void fb_clear(void);

/**
 *  Writes size bytes from the given data to the framebuffer.
 * 
 *  @param data the data to be written
 * 	@param size the size of the data
*/
void fb_write(const char* data, size_t size);

/**
 *  Writes a null terminated string to the framebuffer.
 * 
 *  @param str the string to be written
*/
void fb_writestring(const char* str);

/**
 * 	Writes the given character to the framebuffer.
 * 
 * 	@param c the char to be written
*/
void fb_writechar(char c);
