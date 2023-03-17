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

static char* fb = (char*) FB_MMIO_LOCATION;  	// fb[i*2]:       Code Point
												// fb[i*2 + 1]:
												//		bit  7:	  Blink Bit
												//      bits 6-4: Background Color
												//      bits 3-0: Foreground Color

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

/* Size of the framebuffer in cells */
#define FB_WIDTH            	80
#define FB_HEIGHT            	25
#define FB_CELL_COUNT			FB_WIDTH * FB_HEIGHT

/* Color codes */
#define BLACK                   0
#define WHITE                   15


void cursor_enable(uint8_t cursor_start, uint8_t cursor_end) {
	cursor_start &= 0x1F;   // to make sure that other fields remain untouched

	outb(FB_COMMAND_PORT, CURSOR_START_REGISTER);
	outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xC0) | cursor_start);

	outb(FB_COMMAND_PORT, CURSOR_END_REGISTER);
	outb(FB_DATA_PORT, (inb(FB_DATA_PORT) & 0xE0) | cursor_end);
}

void cursor_disable(void) {
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

void cursor_move(uint16_t pos) {
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, (pos >> 8) & 0xFF);
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, pos & 0xFF);
}

void cursor_init(void) {
	cursor_enable(0, FB_HEIGHT-1);
	cursor_move(0);
}

/** fb_write_cell:
 *  Writes a character with the given foreground and background to character position
 *  in the framebuffer.
 *
 *  @param pos character position in the framebuffer
 *  @param c   character to be written
 *  @param bg  background color
 *  @param fg  foreground color
*/
static void fb_write_cell(unsigned int pos, char c, uint8_t bg, uint8_t fg) {
	/*  Since each character takes up two bytes of space in memory and the index is given in
	 *  single steps, pos must be multiplied by 2 to get the correct position to write the character
	*/
	fb[pos*2] = c;
	fb[pos*2 + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

/**	fb_read_cell:
 * 	Reads and returns the data at the indicated position in the framebuffer
 * 
 * 	@return	the data read at the indicated position from the framebuffer
*/
static uint16_t fb_read_cell(unsigned int pos) {
	uint16_t data = 0;

	data |= fb[pos*2 + 1];
	data |= (fb[pos*2] << 8);

	return data;
}

void fb_clear(void) {
	for(unsigned i = 0; i < FB_WIDTH; i++)
		for(unsigned j = 0; j < FB_HEIGHT; j++)
			fb_write_cell((j*FB_WIDTH + i), 0, 0, 0);
}

/**	fb_scroll_up:
 * 	Simulates a scroll up by copying every line's data to the line above 
 * 	and clearing the last line.
*/
static void fb_scroll_up() {
	// move every line up starting from the second
	for(unsigned line = 1; line < FB_HEIGHT; line++)
		for(unsigned col = 0; col < FB_WIDTH; col++) {
			unsigned old_pos = line*FB_WIDTH + col;
			uint16_t cell_data = fb_read_cell(old_pos);
			fb_write_cell(old_pos - FB_WIDTH, ((cell_data >> 8) & 0xFF), ((cell_data >> 4) & 0xF), (cell_data & 0xF));
		}

	// clear the last line
	for(unsigned i = FB_WIDTH; i > 0; i--)
		fb_write_cell((FB_CELL_COUNT-i), 0, 0, 0);
}

void fb_write(char* str) {
	uint16_t cursor_pos = get_cursor_position();

	char curr_char;
	unsigned i = 0;
	for(; (curr_char = str[i]) != '\0'; i++) {
		unsigned is_newline = curr_char == '\n';

		// if a new line character is received, jump to the framebuffer's next line
		if(is_newline) {
			unsigned next_line = cursor_pos/FB_WIDTH + 1;
			cursor_pos = next_line*FB_WIDTH;
		}

		// if the framebuffer is full, simulate a scroll up
		if(cursor_pos >= FB_CELL_COUNT) {
			fb_scroll_up();
			cursor_pos -= FB_WIDTH;
		}

		if(!is_newline)
			fb_write_cell(cursor_pos++, curr_char, BLACK, WHITE);
	}

	cursor_move(cursor_pos);
}