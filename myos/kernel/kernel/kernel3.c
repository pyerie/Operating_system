#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/multiboot.h>
#include <stdint.h>
#include <string.h>
uint8_t compare(char input[], char cmd[]){
	if (strlen(input) != strlen(cmd)){
		return 1;
	}
	size_t i = 0;
	while (input[i] != '\0' && cmd[i] != '\0'){
		if (input[i] != cmd[i]){
			return 1;
		}
		i++;
	}
	return 0;
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


uint16_t get_cursor_position(void)
{
	uint16_t pos = 0;
	outportb(0x3D4, 0x0F);
	pos |= inportb(0x3D5);
	outportb(0x3D4, 0x0E);
	pos |= ((uint16_t)inportb(0x3D5)) << 8;
	return pos;
}
char* commands[] = {"firstcommand", "setfggreen", "setfgred", "setfgblue", "setfggrey", "end"};
void exec(char cmd[]){
	char* not_recognized = "Not recognized.";
	if (compare(commands[0], cmd) == 0){
		terminal_writestring("Yay!\n");
	} else if (compare(commands[1], cmd) == 0){
		terminal_changefgcolour(VGA_COLOUR_LIGHT_GREEN); // Dummy colour values, create an enum to associate normal langauge colours to VGA entry colour numbers(black = 0, blue = 1, etc.)
	} else if (compare(commands[2], cmd) == 0){
                terminal_changefgcolour(VGA_COLOUR_LIGHT_RED); // Dummy colou>
        } else if (compare(commands[3], cmd) == 0){
                terminal_changefgcolour(VGA_COLOUR_LIGHT_BLUE); // Dummy colou>
        } else if (compare(commands[4], cmd) == 0){
                terminal_changefgcolour(VGA_COLOUR_LIGHT_GREY); // Dummy colou>
        } else {
		terminal_writestring(not_recognized);
	}
}

void kernel_main(uint32_t magic, multiboot_info_t* mbd){
	const size_t VGA_WIDTH = 80;
        terminal_initialize();
	uint64_t rcs = 0;
	int ring;
	asm ("mov %%cs, %0" : "=r" (rcs));
	ring  = (int) (rcs & 3);
	printf("Current ring is: %d\n", ring);
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC){
                printf("Invalid magic: 0x%x\n", magic);
        }
        // Check bit 6 and verify if we have a memory map
        if (!(mbd->flags >> 6 & 0x1)){
                printf("Invalid memory map!");
        }
        int i = 0;
        for (i = 0; i < mbd->mmap_length; i+= sizeof(multiboot_memory_map_t)){
                multiboot_memory_map_t* mmmt = (multiboot_memory_map_t*) (mbd->mmap_addr + i);
                printf("Addr low: %x | Addr high: %x | Length low: %x | Length high: %x | Size: %x | Type: %d\n", mmmt->addr_low, mmmt->addr_high, mmmt->len_low, mmmt->len_high, mmmt->size, mmmt->type);
        }

	// 3 lines allowed
	printf("Kernel V3.00\n");
	// x = horizontal(columns), y = vertical(rows)
	update_cursor(1,6);
	uint16_t pos = get_cursor_position();
	uint8_t column = pos % VGA_WIDTH;
    	uint8_t row = pos / VGA_WIDTH;
	uint8_t input = ' ';
	uint8_t input_upper = ' ';
	char cmd_input[1024];
	uint8_t shift_pressed = 0;
	uint8_t enter_pressed = 0;
	const uint8_t terminal_colour = vga_entry_colour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
	char* cmd1 = "Yay!\nYay\n";
	char* the_end = "end";

	size_t len(){
		size_t i = 0;
		while (compare(commands[i], the_end) != 0){
			i++;
		}
		return i;
	}
	while (1){
		if ((inportb(0x64) & 1) != 0){
			input = inportb(0x60);
			if (!(input & 0x80)){
				if (input == 0x2a){
					shift_pressed = 1;
					continue;
				} else if (input == 0x0e){
					if (strlen(cmd_input) != 0){
						cmd_input[strlen(cmd_input)-1] = '\0';
						terminal_backspace();
					}
					continue;
				} else if (input == 0x1c){
					enter_pressed = 1;
				}
				input = convert(input);
				if (shift_pressed == 1){
					input_upper = input - 32;
					terminal_putchar(input_upper);
					shift_pressed = 0;
				} else if (enter_pressed == 1){
					terminal_putchar('\n');
					exec(cmd_input);
					terminal_putchar('\n');
					memset(cmd_input, '\0', sizeof(cmd_input));
					enter_pressed = 0;
				} else {
					cmd_input[strlen(cmd_input)] = input;
					cmd_input[strlen(cmd_input)+1] = '\0';
					terminal_putchar(input);
				}
				column++;
			} else {
				// Check if shift is released
				// 0x7f = 11111111. Performing the AND operation on this only removes the MSB(Most Significant Bit), giving us the original scancode
				if ((input & 0x7f) == 0x2a){
					shift_pressed = 0;
				}
			}
		}
		io_wait();

	}
}
