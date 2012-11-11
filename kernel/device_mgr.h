// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_MGR_H
#define WAGTAIL_DEVICE_MGR_H

#include "utils.h"

namespace wagtail
{
	class device;

	class device_mgr final
	{
		public:
			// Number of major device nodes to preallocate space for.
			// FIXME: At the moment, no more than this number of devices
			// FIXME: can be registered.
			static const int PREALLOCATE_DEVICES = 32;

			// Gets the device manager instance:
			static device_mgr * get() { return devmgr; }

			unsigned int register_device(device * dev);
			void unregister_device(unsigned int number);

			device * get_device_by_name(const char * name) const;
			device * get_device_by_number(unsigned int number) const { return devices[number]; }
		private:
			device_mgr();
			~device_mgr() { delete[] devices; }

			unsigned int next_devnum = 0;

			device ** devices;
			static device_mgr * devmgr;
	};
}

#endif

