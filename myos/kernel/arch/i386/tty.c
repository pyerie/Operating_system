#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_colour;
static uint16_t* terminal_buffer;
enum vga_colour fg = VGA_COLOUR_LIGHT_GREY;
enum vga_colour bg = VGA_COLOUR_BLACK;
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_colour = vga_entry_colour(fg, bg);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_colour);
		}
	}
}

void terminal_setcolour(uint8_t colour) {
	terminal_colour = colour;
}

void terminal_changefgcolour(enum vga_colour colour){
	fg = colour;
	terminal_setcolour(vga_entry_colour(fg, bg));
}


void terminal_putentryat(unsigned char c, uint8_t colour, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, colour);
}

void outportb(uint16_t port, uint8_t value){
	asm volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

uint8_t inportb(uint16_t port){
	uint8_t ret;
	asm volatile ("inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
	return ret;
}
void update_cursor(int x, int y){
        const size_t VGA_WIDTH = 80;
        uint16_t pos = y * VGA_WIDTH + x;
        outportb(0x3D4, 0x0F);
        outportb(0x3D5, (uint8_t) (pos & 0xFF));
        outportb(0x3D4, 0x0E);
        outportb(0x3D5, (uint8_t) ((pos>>8) & 0xFF));
}

void scroll_up(){
	for (size_t y = 0; y < VGA_HEIGHT; y++){
		memcpy(VGA_MEMORY + (y-1) * VGA_WIDTH, VGA_MEMORY + y * VGA_WIDTH, VGA_WIDTH * sizeof(uint16_t) );
	}

	for (size_t x = 0; x < VGA_WIDTH; x++){
		terminal_putentryat(' ', terminal_colour, x, VGA_HEIGHT - 1);
	}
}

void terminal_putchar(char c) {
	unsigned char uc = c;
	if (c == '\n'){
		terminal_column = 0;
		if (++terminal_row >= VGA_HEIGHT){
			terminal_row++;
		}
	} else {
		terminal_putentryat(uc, terminal_colour, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT)
				terminal_row = 0;
		}
	}
	if (terminal_row == VGA_HEIGHT-1){
		scroll_up();
		terminal_row = VGA_HEIGHT - 2;
	}
	update_cursor(terminal_column, terminal_row);
}

void terminal_backspace(){
	if (terminal_row > 9){
		if (terminal_column > 0){
			terminal_column--;
			terminal_putchar(' ');
			terminal_column--;
			update_cursor(terminal_column, terminal_row);
		} else if (terminal_column == 0){
			terminal_row--;
			terminal_putentryat(' ', terminal_colour, VGA_WIDTH-1, terminal_row);
			terminal_column = VGA_WIDTH - 1; // WIDTH - 1
			update_cursor(terminal_column, terminal_row);
		}
	} else if (terminal_row == 9 && terminal_column > 0){
		terminal_column--;
		terminal_putchar(' ');
		terminal_column--;
		update_cursor(terminal_column, terminal_row);
	}
}
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
