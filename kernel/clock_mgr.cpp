// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "clock_mgr.h"
#include "kernel.h"

using namespace wagtail;

// Virtual base address for the Core CM module:
void * clock_mgr::core_cm_base = mmu::map_device(reinterpret_cast<void *>(0x48004000), 8192);

// Enables a functional clock:
void clock_mgr::enable(functional_clocks clock)
{
	unsigned int bitmask = (unsigned int) clock;
	io::or_register(bitmask, core_cm_base, fclken1_core::offset);
}

// Enables an interface clock:
void clock_mgr::enable(interface_clocks clock)
{
	unsigned int bitmask = (unsigned int) clock;
	io::or_register(bitmask, core_cm_base, iclken1_core::offset);
}

// Disables a functional clock:
void clock_mgr::disable(functional_clocks clock)
{
	unsigned int bitmask = ~(unsigned int) clock;
	io::and_register(bitmask, core_cm_base, fclken1_core::offset);
}

// Disables an interface clock:
void clock_mgr::disable(interface_clocks clock)
{
	unsigned int bitmask = ~(unsigned int) clock;
	io::and_register(bitmask, core_cm_base, iclken1_core::offset);
}

