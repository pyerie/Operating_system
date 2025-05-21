#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
#include "vga.h"
void terminal_putentryat(unsigned char, uint8_t, size_t, size_t);
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char *data, size_t size);
void terminal_writestring(const char *data);
void update_cursor(int, int);
void outportb(uint16_t, uint8_t);
uint8_t inportb(uint16_t);
void terminal_backspace();
void terminal_setcolour(uint8_t);
void terminal_changefgcolour(enum vga_colour colour);
void terminal_changebgcolour(enum vga_colour colour);
#endif
