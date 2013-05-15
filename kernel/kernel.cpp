// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"

using namespace wagtail;

kostream & kernel::message()
{
	return uart::get(2);
}

extern "C" void kernel::kernel_main()
{
	// Print a heading before initializing:
	message() << "Welcome to Wagtail OS v" << version::major << '.' << version::minor
		<< " :-)" << kstream::newline;

	// Initialize the syscall handler and set up the syscalls:
	syscall_handler::initialize();

	// Initialize the scheduler:
	scheduler::initialize();

	// Start the initialization thread:
	scheduler::get()->schedule(new kernel::init_thread);

	// Start the scheduler:
	scheduler::get()->start();

	while(true) asm volatile("wfi\n\t");
}

extern "C" void kernel::panic()
{
	// Let's hope the message() function still works when panicing :-)
	message() << kstream::newline;
	message() << "*** KERNEL PANIC ***" << kstream::newline;
	message() << "A fatal error occurred, and Wagtail cannot recover." << kstream::newline;
	message() << "Reboot and try again." << kstream::newline << kstream::newline;

	// Disable all interrupts and hang forever:
	asm volatile("cpsid aif\n\t");
	while(true) asm volatile("wfi\n\t");
}

int kernel::init_thread::run()
{
	// Initialize kernel here.

	kernel::message() << "Kernel initialized!" << kstream::newline << kstream::newline;
	return 0;
}

