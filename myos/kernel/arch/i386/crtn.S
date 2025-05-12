.section .init
	/* GCC will put contents of crtend.o's init func here */
	popl %ebp
	ret

.section .fini
	/* GCC will put contents of crtend.o's fini func here */
	popl %ebp
	ret

