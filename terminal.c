/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

enum status {
	NORMAL,
	ESCAPE,
	ESCAPE_CONTROL,
};

struct {
	size_t row;
	size_t column;
	uint8_t color;
	uint8_t state;
	uint8_t saved_row;
	uint8_t saved_column;
	uint8_t escape_n;
} *cursor;

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;

uint16_t * const terminal_buffer = (uint16_t *) 0xB8000;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void erase_line(void) {
	//for (size_t y = 0; y < VGA_HEIGHT; y++) {
	//	const size_t index = y * VGA_WIDTH;
	//}
}
 
void terminal_initialize(void) 
{
	cursor->row = 0;
	cursor->column = 0;
	cursor->color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	cursor->state = NORMAL;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', cursor->color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	cursor->color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	switch (cursor->state) {
		case NORMAL:
			if (c == '\x1B') {
				cursor->state = ESCAPE;
				break;
			} else if (c != '\n')
				terminal_putentryat(c, cursor->color, cursor->column, cursor->row);
			else
				cursor->column = VGA_WIDTH - 1;

			if (++cursor->column == VGA_WIDTH) {
				cursor->column = 0;
				if (++cursor->row == VGA_HEIGHT)
					cursor->row = 0;
			}
			break;

		case ESCAPE:
			if (c == '[')
				cursor->state = ESCAPE_CONTROL;
			else
				cursor->state = NORMAL;	
			break;

		case ESCAPE_CONTROL:
			if (c == 's') {
				cursor->state = NORMAL;
				cursor->saved_column = cursor->column;
				cursor->saved_row = cursor->row;
			} else if (c == 'u') {
				cursor->state = NORMAL;
				cursor->column = cursor->saved_column;
				cursor->row = cursor->saved_row;
			}
			
			break;
	}
}
 
void terminal_writestring(const char* data) 
{
	for (size_t i = 0; data[i] != '\0'; i++)
		terminal_putchar(data[i]);
}