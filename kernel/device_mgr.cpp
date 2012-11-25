// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "device_mgr.h"
#include "kernel.h"

using namespace wagtail;

device_mgr * device_mgr::devmgr = new device_mgr;

unsigned int device_mgr::register_device(device * dev)
{
	unsigned int retval = next_devnum++;

	if(get_device_by_name(dev->get_name()))
	{
		kernel::message() << "Cannot register device " << dev->get_name()
			<< ": a device with the same name is already registered!"
			<< kstream::newline;
		return -1;
	}

	devices[retval] = dev;
	return retval;
}

void device_mgr::unregister_device(unsigned int number)
{
	devices[number] = nullptr;
}

device * device_mgr::get_device_by_name(const kstring & name)
{
	for(int i = 0; i < devices.get_capacity(); ++i)
		if(devices[i] != nullptr && devices[i]->get_name() == name)
			return devices[i];
	return nullptr;
}

device_mgr::device_mgr() : devices(PREALLOCATE_DEVICES)
{
	for(int i = 0; i < PREALLOCATE_DEVICES; ++i)
		devices[i] = nullptr;
}

