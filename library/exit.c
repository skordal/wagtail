// Wagtail OS C Library
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "include/wagtail/syscalls.h"

#include "include/stdlib.h"
#include "include/unistd.h"

/**
 * Calls the exit syscall.
 * @param status the program status message.
 */
void __attribute((noreturn, naked)) __syscall_exit(int status)
{
	asm volatile(
		"svc %[syscall_num]\n\t"
		"1:\n\t"
		"wfi\n\t"
		"b.n 1b\n\t"
		:: [syscall_num] "I" (SYSCALL_EXIT)
	);
}

/**
 * Terminates the current process.
 * @param status the program status message.
 * @todo Call functions registered by `atexit()` in reverse order.
 * @todo Flush and close open streams.
 */
void __attribute((noreturn)) exit(int status)
{
	__syscall_exit(status);
}


