#include <stdio.h>
#include <kernel/tty.h>

void kernel_main(void){
	terminal_initialize();
	printf("Hello, world!\n");
	printf("Kernel V3.00\n TEsting....");
}
