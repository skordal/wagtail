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

	/**
	 * Device manager. This class keeps track of the various devices on the system, so
	 * that they can be looked up using their name or a number.
	 *
	 * @todo Use rbtree to store devices in.
	 */
	class device_mgr final
	{
		public:
			/** Number of devices to preallocate room for in the device array. */
			static const int PREALLOCATE_DEVICES = 32;

			/**
			 * Gets the global device manager instance.
			 * @return the global device manager instance.
			 */
			static device_mgr * get() { return devmgr; }

			/**
			 * Registers a device.
			 * @param dev the device manager.
			 * @return the device number.
			 */
			unsigned int register_device(device * dev);
			/**
			 * Unregisters a device.
			 * @param number the number of the device.
			 */
			void unregister_device(unsigned int number);

			/**
			 * Gets a device by its name.
			 * @param name the name of the device.
			 * @return a pointer to the device, or `nullptr` if no such device was found.
			 */
			device * get_device_by_name(const kstring & name);
			/**
			 * Gets a device by its device number.
			 * @param number the device number.
			 * @return a pointer to the device, or `nullptr` if no such device was found.
			 */
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

