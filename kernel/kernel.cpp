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

	// Initialize the SD card:
	sd_card::initialize();

	// Print some memory manager debug info, which makes it easier
	// to spot memory manager bugs:
	mm::print_debug_info(message());

	// Kernel loop:
	while(true) asm volatile("wfi\n\t");
}

extern "C" void kernel::panic()
{
	// Hope the message() function still works when panicing :-)
	message() << kstream::newline;
	message() << "*** KERNEL PANIC ***" << kstream::newline;
	message() << "A fatal error occurred, and Wagtail cannot recover." << kstream::newline;
	message() << "Please do not panic (like the kernel), but reset your computer."
		<< kstream::newline << kstream::newline;

	// Disable all interrupts and hang forever:
	asm volatile("cpsid aif\n\t");
	while(true) asm volatile("wfi\n\t");
}

