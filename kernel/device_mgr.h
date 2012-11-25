// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_MGR_H
#define WAGTAIL_DEVICE_MGR_H

#include "expandable_array.h"
#include "kstring.h"
#include "utils.h"

namespace wagtail
{
	class device;

	// The device manager class. This class allows for adding and removing devices,
	// and for keeping track of them using their names or numbers.
	class device_mgr final
	{
		public:
			// Number of major device nodes to preallocate space for.
			static const int PREALLOCATE_DEVICES = 32;

			// Gets the device manager instance:
			static device_mgr * get() { return devmgr; }

			unsigned int register_device(device * dev);
			void unregister_device(unsigned int number);

			device * get_device_by_name(const kstring & name);
			device * get_device_by_number(unsigned int number) { return devices[number]; }
		private:
			device_mgr();
			~device_mgr() {}

			unsigned int next_devnum = 0;

			expandable_array<device *> devices;
			static device_mgr * devmgr;
	};
}

#endif

