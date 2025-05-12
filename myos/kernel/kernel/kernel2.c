#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <stdint.h>



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

void io_wait(void){
	asm volatile ("outb %%al, $0x80" : : "a"(0) );
}

uint8_t convert(uint8_t key){
	switch (key){
	case 0x2:
		return '1';
		break;

	case 0x3:
		return '2';
		break;
	case 0x4:
		return '3';
		break;
	case 0x5:
		return '4';
		break;
	case 0x6:
		return '5';
		break;
	case 0x7:
		return '6';
		break;
	case 0x8:
		return '7';
		break;
	case 0x9:
		return '8';
		break;
	case 0xa:
		return '9';
		break;
	case 0xb:
		return '0';
		break;
	case 0xc:
		return '-';
		break;
	case 0xd:
		return '=';
		break;
	case 0xe:
		return '\b';
		break;
	case 0xf:
		return '\t';
		break;
	case 0x10:
		return 'q';
		break;
	case 0x11:
		return 'w';
		break;
	case 0x12:
		return 'e';
		break;
	case 0x13:
		return 'r';
		break;
	case 0x14:
		return 't';
		break;
	case 0x15:
		return 'y';
		break;
	case 0x16:
		return 'u';
		break;
	case 0x17:
		return 'i';
		break;
	case 0x18:
		return 'o';
		break;
	case 0x19:
		return 'p';
		break;
	case 0x1e:
		return 'a';
		break;
	case 0x1f:
		return 's';
		break;
	case 0x20:
		return 'd';
		break;
	case 0x21:
		return 'f';
		break;
	case 0x22:
		return 'g';
		break;
	case 0x23:
		return 'h';
		break;
	case 0x24:
		return 'j';
		break;
	case 0x25:
		return 'k';
		break;
	case 0x26:
		return 'l';
		break;
	case 0x2c:
		return 'z';
		break;
	case 0x2d:
		return 'x';
		break;
	case 0x2e:
		return 'c';
		break;
	case 0x2f:
		return 'v';
		break;
	case 0x30:
		return 'b';
		break;
	case 0x31:
		return 'n';
		break;
	case 0x32:
		return 'm';
		break;
	case 0x39:
		return ' ';
		break;

	}
}

void update_cursor(int x, int y){
	const size_t VGA_WIDTH = 80;
	uint16_t pos = y * VGA_WIDTH + x;
	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (uint8_t) (pos & 0xFF));
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (uint8_t) ((pos>>8) & 0xFF));
}

uint16_t get_cursor_position(void)
{
	uint16_t pos = 0;
	outportb(0x3D4, 0x0F);
	pos |= inportb(0x3D5);
	outportb(0x3D4, 0x0E);
	pos |= ((uint16_t)inportb(0x3D5)) << 8;
	return pos;
}
void kernel_main(void){
	const size_t VGA_WIDTH = 80;
	terminal_initialize();
	printf("Hello, world!\n");
	printf("Kernel V3.00\n TEsting....");
	printf("\n");
	// x = horizontal(columns), y = vertical(rows)
	update_cursor(1,3);
	uint16_t pos = get_cursor_position();
        uint8_t column = pos % VGA_WIDTH;
        uint8_t row = pos / VGA_WIDTH;
	uint8_t input = ' ';
	uint8_t input_upper = ' ';
	uint8_t shift_pressed = 0;
	const uint8_t terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
	while (1){
		if ((inportb(0x64) & 1) != 0){
			input = inportb(0x60);
			if (!(input & 0x80)){
				if (input == 0x2a){
					shift_pressed = 1;
					continue;
				} else if (input == 0x0e){
					if (column > 0){
						column--;
						terminal_putentryat(' ', terminal_colour, column, row);
						update_cursor(column, row);
					}
					continue;
				}
				input = convert(input);
				if (shift_pressed == 1){
					input_upper = input - 32;
					printf("%c", input_upper);
					shift_pressed = 0;
				} else {
					printf("%c", input);
				}
				column++;
				update_cursor(column-1, row);
			} else {
				// Check if shift is released
				// 0x7f = 11111111. Performing the AND operation on this only removes the MSB(Most Significant Bit), giving us the original scancode
				if ((input & 0x7f) == 0x2a){
					shift_pressed = 0;
				}
			}
		}

	}
}
