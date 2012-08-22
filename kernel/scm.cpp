// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "scm.h"

using namespace wagtail;

scm * scm::global_scm = nullptr;

void scm::padconf_register::enable_input_0(bool enable)
{
	if(enable)
		register_value |= 1 << 8;
	else
		register_value &= ~(1 << 8);
}

void scm::padconf_register::enable_input_1(bool enable)
{
	if(enable)
		register_value |= 1 << 24;
	else
		register_value &= ~(1 << 24);
}

void scm::initialize()
{
	global_scm = new scm;
}

scm::scm() : virtual_base(mmu::map_device(reinterpret_cast<void *>(0x48002000), 4096))
{
	kernel::message() << "Initializing system control module... ";
	padconfs = reinterpret_cast<padconf_register *>((unsigned int) virtual_base + 0x30);
	kernel::message() << "ok" << kstream::newline;
}

scm::padconf_register * scm::get_padconf_register(unsigned int index)
{
	if(index > 370)
		return nullptr;
	else
		return reinterpret_cast<scm::padconf_register *>((unsigned int) padconfs
			+ (index * sizeof(scm::padconf_register)));
}

