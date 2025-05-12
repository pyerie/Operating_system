#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn))

void abort(void) {
	#if defined(__is_libk)
		printf("Kernel: aborted!\n");
			asm volatile("hlt");
	#else
		printf("Abort()\n");
	#endif
		while(1) { }
		__builtin_unreachable();
}
