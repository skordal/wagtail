// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "aborts.h"

using namespace wagtail;

// Handles a data abort:
void wagtail::data_abort()
{
	void * fault_address;
	unsigned int fault_status;

	// Read the fault status register (DFSR):
	asm volatile("mrc p15, 0, %[status_reg], c5, c0, 0\n\t"
		: [status_reg] "=r" (fault_status));

	// Read the address causing the fault (DFAR):
	asm volatile("mrc p15, 0, %[address_reg], c6, c0, 0\n\t"
		: [address_reg] "=r" (fault_address));

	kernel::message() << kstream::newline << "Data abort while trying to ";
	if(fault_status & 1 << 11)
		kernel::message() << "write ";
	else
		kernel::message() << "read ";
	kernel::message() << fault_address << kstream::newline;
}

void wagtail::prefetch_abort()
{
	void * fault_address;

	// Read the fault address (IFAR):
	asm volatile("mrc p15, 0, %[fault_address], c6, c0, 2\n\t"
		: [fault_address] "=r" (fault_address));

	kernel::message() << kstream::newline << "Prefetch abort when trying to access "
		<< fault_address << kstream::newline;
}

