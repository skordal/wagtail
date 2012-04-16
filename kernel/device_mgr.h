// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_MGR_H
#define WAGTAIL_DEVICE_MGR_H

#include "device.h"
#include "klist.h"

namespace wagtail
{
	class device_mgr
	{
		public:
			// Initializes the device manager:
			static void initialize();
			// Gets a reference to the kernel device manager:
			static device_mgr & get() { return *kernel_devmgr; }

			// Registers a device with the device manager:
			void register_device(device * dev);
			// Unregisters a device with the device manager:
			void unregister_device(device * dev);
		private:
			// List of devices: (TODO: Find a better data structure for devices)
			klist<device *> device_list;

			// The global device manager:
			static device_mgr * kernel_devmgr;
	};
}

#endif

