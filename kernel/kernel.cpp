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

	// Initialize the IRQ handler:
	irq_handler::initialize();

	// Initialize the system control module:
	scm::initialize();

	// Initialize the SD card controller:
	sd::initialize();

	// Initialize the VFS module:
	vfs::initialize();

	// Mount the root directory:
	kernel::message() << "Mounting sd0p0 as root..." << kstream::newline;
	filesystem * root = vfs::get()->mount("/", (partition *) device_mgr::get()->get_device_by_name("sd0p0"));
	if(root == nullptr)
	{
		kernel::message() << "*** Fatal error: Could not mount root filesystem!" << kstream::newline;
		panic();
	}

	kernel::message() << "Kernel halting." << kstream::newline;
}

extern "C" void kernel::panic()
{
	// Let's hope the message() function still works when panicing :-)
	message() << kstream::newline;
	message() << "*** KERNEL PANIC ***" << kstream::newline;
	message() << "A fatal error occurred, and Wagtail cannot recover." << kstream::newline;
	message() << "Reboot and try again."
		<< kstream::newline << kstream::newline;

	// Disable all interrupts and hang forever:
	asm volatile("cpsid aif\n\t");
	while(true) asm volatile("wfi\n\t");
}

