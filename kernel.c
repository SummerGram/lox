#include <stdint.h>
#include <stddef.h>
#include "terminal.c"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	const char *a = "\x1B[s";
	const char *b = "\x1B[u";
 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring(a);
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring(b);
	terminal_writestring("ABCD");
}