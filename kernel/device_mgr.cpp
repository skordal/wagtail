// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "device_mgr.h"
#include "kernel.h"

using namespace wagtail;

device_mgr * device_mgr::devmgr = new device_mgr;

unsigned int device_mgr::register_dev(device * dev)
{
	unsigned int retval = next_devnum;

	// FIXME: This code determines the next device number to assign, but it does
	// FIXME: not take the maximum number of allocated devices into account.
	if(devices[next_devnum + 1] == nullptr)
		++next_devnum;
	else {
		for(int i = next_devnum + 1; i < PREALLOCATE_DEVICES; ++i)
		{
			if(devices[i] == nullptr)
			{
				next_devnum = i;
				break;
			}
		}
	}

	devices[retval] = dev;
	return retval;
}

void device_mgr::unregister_dev(unsigned int number)
{
	devices[number] = nullptr;
	if(number < next_devnum)
		next_devnum = number;
}

device_mgr::device_mgr()
{
	devices = new device*[PREALLOCATE_DEVICES];
	for(int i = 0; i < PREALLOCATE_DEVICES; ++i)
		devices[i] = nullptr;
}

