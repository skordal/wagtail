// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "device_mgr.h"

using namespace wagtail;

// The global device manager object:
device_mgr * device_mgr::kernel_devmgr = 0;

// Initializes the kernel device manager:
void device_mgr::initialize()
{
	kernel::message() << "Initializing device manager: ";
	kernel_devmgr = new device_mgr;
	kernel::message() << "ok" << kstream::newline;
}

// Registers a device with the device manager:
void device_mgr::register_device(device * dev)
{
	kernel::message() << "Registering device: " << *dev << kstream::newline;
}

// Unregisters a device with the device manager:
void device_mgr::unregister_device(device * dev)
{
	kernel::message() << "Unregistering device: " << *dev << kstream::newline;
}

