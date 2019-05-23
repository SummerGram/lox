#include "terminal.h"

const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;

uint16_t * const terminal_buffer = (uint16_t *) 0xB8000;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
void terminal_initialize(void) {
	cursor->row = 0;
	cursor->column = 0;
	cursor->color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	cursor->state = NORMAL;
	cursor->saved_row = 0;
	cursor->saved_column = 0;
	cursor->value = 0;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', cursor->color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) {
	cursor->color = color;
}
 
void terminal_putentryat(char c, uint8_t color, int x, int y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

int screen_index(int y, int x) {
	return y * VGA_WIDTH + x;
}

void erase(int start, int end) {
	for (; start <= end; start++)
		terminal_buffer[start] = vga_entry(' ', cursor->color);
}

void terminal_putchar(char c) {
	switch (cursor->state) {
		case NORMAL:
			cursor->value = 0;
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
			if (c == 'A') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->row = (cursor->row - cursor->value < 0) ? 0 : cursor->row - cursor->value;
			} else if (c == 'B') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->row = (cursor->row + cursor->value >= (int) VGA_HEIGHT) ? (int) VGA_HEIGHT - 1 : cursor->row + cursor->value;
			} else if (c == 'C') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->column = (cursor->column + cursor->value >= (int) VGA_WIDTH) ? (int) VGA_WIDTH - 1 : cursor->column + cursor->value;
			} else if (c == 'D') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->column = (cursor->column - cursor->value < 0) ? 0 : cursor->column - cursor->value;
			} else if (c == 'E') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->column = 0;
				cursor->row = (cursor->row + cursor->value >= (int) VGA_HEIGHT) ? (int) VGA_HEIGHT - 1 : cursor->row + cursor->value;
			} else if (c == 'F') {
				cursor->value = 1;
				cursor->state = NORMAL;
				cursor->column = 0;
				cursor->row = (cursor->row - cursor->value < 0) ? 0 : cursor->row - cursor->value;
			} else if (c == 's') {
				cursor->state = NORMAL;
				cursor->saved_column = cursor->column;
				cursor->saved_row = cursor->row;
			} else if (c == 'u') {
				cursor->state = NORMAL;
				cursor->column = cursor->saved_column;
				cursor->row = cursor->saved_row;
			} else if (c >= '0' && c <= '9') {
				cursor->state = FVALUE;
				cursor->value = c - '0';
			} else if (c == 'J') {
				erase(screen_index(cursor->row, cursor->column), screen_index(VGA_HEIGHT, 0) - 1);

				cursor->state = NORMAL;
			} else if (c == 'K') {
				erase(screen_index(cursor->row, cursor->column), screen_index(cursor->row + 1, 0) - 1);

				cursor->state = NORMAL;
			} else if (c == ';') {
				cursor->state = SVALUE;
				cursor->second_value = 0;
			}
			else
				cursor->state = NORMAL;
			break;

		case FVALUE:
			if (c == 'A') {
				if (cursor->value == 0)
					cursor->value = 1;

				cursor->state = NORMAL;
				cursor->row = (cursor->row - cursor->value < 0) ? 0 : cursor->row - cursor->value;
			} else if (c == 'B') {
				if (cursor->value == 0)
					cursor->value = 1;

				cursor->state = NORMAL;
				cursor->row = (cursor->row + cursor->value >= (int) VGA_HEIGHT) ? (int) VGA_HEIGHT - 1 : cursor->row + cursor->value;
			} else if (c == 'C') {
				if (cursor->value == 0)
					cursor->value = 1;

				cursor->state = NORMAL;
				cursor->column = (cursor->column + cursor->value >= (int) VGA_WIDTH) ? (int) VGA_WIDTH - 1 : cursor->column + cursor->value;
			} else if (c == 'D') {
				if (cursor->value == 0)
					cursor->value = 1;

				cursor->state = NORMAL;
				cursor->column = (cursor->column - cursor->value < 0) ? 0 : cursor->column - cursor->value;
			} else if (c == 'E') {
				cursor->state = NORMAL;
				cursor->column = 0;
				cursor->row = (cursor->row + cursor->value >= (int) VGA_HEIGHT) ? (int) VGA_HEIGHT - 1 : cursor->row + cursor->value;
			} else if (c == 'F') {
				cursor->state = NORMAL;
				cursor->column = 0;
				cursor->row = (cursor->row - cursor->value < 0) ? 0 : cursor->row - cursor->value;
			} else if (c == 'K') {
				if (cursor->value == 0)
					erase(screen_index(cursor->row, cursor->column), screen_index(cursor->row + 1, 0) - 1);
				else if (cursor->value == 1)
					erase(screen_index(cursor->row - 1, VGA_WIDTH), screen_index(cursor->row, cursor->column));
				else if (cursor->value == 2)
					erase(screen_index(cursor->row - 1, VGA_WIDTH), screen_index(cursor->row + 1, 0) - 1);

				cursor->state = NORMAL;
			} else if (c == 'J') {
				if (cursor->value == 0)
					erase(screen_index(cursor->row, cursor->column), screen_index(VGA_HEIGHT, 0) - 1);
				else if (cursor->value == 1)
					erase(0, screen_index(cursor->row, cursor->column));
				else if (cursor->value == 2)
					erase(0, screen_index(VGA_HEIGHT, 0) - 1);
			
				cursor->state = NORMAL;
			} else if (c >= '0' && c <= '9')
				cursor->value = cursor->value * 10 + c - '0';
			else if (c == ';') {
				cursor->state = SVALUE;
				cursor->second_value = 0;
			} else
				cursor->state = NORMAL;

			break;

		case SVALUE:
			if (c >= '0' && c <= '9')
				cursor->second_value = cursor->second_value * 10 + c - '0';
			else if (c == 'H') {
				if (cursor->value == 0)
					cursor->value = 1;
				if (cursor->second_value == 0)
					cursor->second_value = 1;

				cursor->value -= 1;
				cursor->second_value -= 1;

				cursor->row = cursor->value;
				cursor->column = cursor->second_value;
				cursor->state = NORMAL;
			} else
				cursor->state = NORMAL;

			break;
	}
}
 
void terminal_writestring(const char* data) {
	for (size_t i = 0; data[i] != '\0'; i++)
		terminal_putchar(data[i]);
}