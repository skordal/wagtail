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

	// Test code for the filesystem interface:
	partition * fat_partition = (partition *) device_mgr::get()->get_device_by_name("sd0p0");
	if(fat_partition == nullptr)
		kernel::message() << "sd0p0 is not a valid device/partition!" << kstream::newline;
	else {
		kernel::message() << "sd0p0 is a valid partition/device, going to test the filesystem interface on it..."
			<< kstream::newline;
		filesystem * fs = filesystem::initialize(fat_partition);
		if(fs == nullptr)
			kernel::message() << "Cannot initialize filesystem driver for sd0p0!" << kstream::newline;
		else {
			kernel::message() << "sd0p0 has volume label \"" << fs->get_label() << "\"" << kstream::newline;
			kernel::message() << "sd0p0 free space: " << (int) fs->get_free_space() << kstream::newline;

			direntry * testdir = fs->read_directory("/");
			if(testdir == nullptr)
				kernel::message() << "Cannot read directory /, directory does not exist"
					<< kstream::newline;
			else {
				direntry * current = testdir;

				// Print the directory listing:
				kernel::message() << "Files in root directory of sd0p0:" << kstream::newline;
				kernel::message() << "Type\tName\t\tSize" << kstream::newline;
				do {
					switch(current->get_type())
					{
						case direntry::type::directory:
							kernel::message() << "Dir\t";
							break;
						case direntry::type::file:
							kernel::message() << "File\t";
							break;
						default:
							kernel::message() << "Other\t";
							break;
					}

					kernel::message() << current->get_name() << "\t"
						<< (int) current->get_size() << " b" << kstream::newline;

					direntry * next = current->get_next();
					delete current;
					current = next;
				} while(current != nullptr);
			}
		}
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

