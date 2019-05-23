#include <stdint.h>
#include <stddef.h>
#include "terminal.h"

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
	const char *a = "\x1B[2A";
	const char *b = "\x1B[s";
	const char *c = "\x1B[u";
	const char *d = "\x1B[;3H";
 
	/* Newline support is left as an exercise. */
	//while (1) {
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring("ABCD");
	terminal_writestring(b);
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring("ABCD");
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring("Hello, kernel World!\ns");
	terminal_writestring(c);
	terminal_writestring("X");
	//terminal_writestring(a);
	terminal_writestring(d);
	terminal_writestring("EFGH");
}