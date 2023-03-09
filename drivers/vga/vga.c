/**
 * A driver for Video Graphics Array
 * Taken partially from https://littleosbook.github.io/ and https://wiki.osdev.org/Text_UI
 * 
 * Refer to:
 * https://en.wikipedia.org/wiki/VGA_text_mode
 * https://wiki.osdev.org/Text_UI
 * http://www.osdever.net/FreeVGA/home.htm
 * 
 * @author Samuel Pires
*/

#include "vga.h"
#include "../../types.h"
#include "../../io.h"

/* Framebuffer memory-mapped I/O location */
#define FB_MMIO_LOCATION        0xB8000;

/* I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* I/O port commands */
#define FB_HIGH_BYTE_COMMAND    0xE
#define FB_LOW_BYTE_COMMAND     0xF

/* Registers */
#define CURSOR_START_REGISTER   0xA		// bit  5:   Cursor Disable
										// bits 4-0: Cursor Scan Line Start

#define CURSOR_END_REGISTER     0xB     // bits 6-5: Cursor Skew
										// bits 4-0: Cursor Scan Line End

/* Cursor scanline values */
#define MIN_SCANLINE            0
#define MAX_SCANLINE            15

/* Color codes */
#define BLACK                   0
#define WHITE                   15


void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	cursor_start &= 0x1F;   // to make sure that other fields remain untouched

	outb(FB_COMMAND_PORT, CURSOR_START_REGISTER);
	outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xC0) | cursor_start);

	outb(FB_COMMAND_PORT, CURSOR_END_REGISTER);
	outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xE0) | cursor_end);
}

void disable_cursor(void) {
	outb(FB_COMMAND_PORT, CURSOR_START_REGISTER);
	outb(FB_DATA_PORT, 0x20);
}

/** get_cursor_position:
 *  Returns the cursor's position.
 * 
 *  @return the cursor's position
*/
static uint16_t get_cursor_position(void) {
	uint16_t pos = 0;
	
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	pos |= inb(FB_DATA_PORT);
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	pos |= ((uint16_t) inb(FB_DATA_PORT)) << 8;

	return pos;
}

void move_cursor(uint16_t pos) {
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, (pos >> 8) & 0xFF);
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, pos & 0xFF);
}

void init_cursor(void) {
	enable_cursor(MIN_SCANLINE, MAX_SCANLINE);
	move_cursor(0);
}

/** fb_write_cell:
 *  Writes a character with the given foreground and background to character position i
 *  in the framebuffer.
 *
 *  @param i  character position in the framebuffer
 *  @param c  character to be written
 *  @param bg background color
 *  @param fg foreground color
*/
static void fb_write_cell(unsigned int i, char c, uint8_t bg, uint8_t fg) {
	char* fb = (char*) FB_MMIO_LOCATION;    // fb[i*2]:       Code Point
											// fb[i*2 + 1]:
											//		bit  7:	  Blink Bit
											//      bits 6-4: Background Color
											//      bits 3-0: Foreground Color
 
	/*  Since each character takes up two bytes of space in memory and the index is given in
	 *  single steps, i must be multiplied by 2 to get the correct position to write the character
	*/
	fb[i*2] = c;
	fb[i*2 + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_write(char* buf, unsigned int len) {
	uint16_t cursor_pos = get_cursor_position();

	for(unsigned i = 0; i < len; i++)
		fb_write_cell(cursor_pos + i, buf[i], BLACK, WHITE);

	move_cursor(cursor_pos + len);
}