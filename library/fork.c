// Wagtail OS C Library
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "include/wagtail/syscalls.h"
#include "include/unistd.h"

/** Calls the `fork` syscall. */
pid_t __attribute((naked)) __syscall_fork()
{
	asm volatile(
		"svc %[syscall_num]\n\t"
		"bx lr\n\t"
		:: [syscall_num] "I" (SYSCALL_FORK)
	);
}

/**
 * Forks a process.
 * @return the PID of the child process for the parent process and 0 for the child process.
 */
pid_t fork()
{
	return __syscall_fork();
}

