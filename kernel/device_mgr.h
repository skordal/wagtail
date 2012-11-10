// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_MGR_H
#define WAGTAIL_DEVICE_MGR_H

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

			// Registers a device with the device manager. A device number is
			// returned. These numbers are dynamically allocated, so they may
			// not be the same each time the same device is registered.
			static unsigned int register_device(device * dev)
				{ return get()->register_dev(dev); }
			// Unregisters a device by its device number:
			static void unregister_device(unsigned int number)
				{ get()->unregister_dev(number); }
			static device * get_device(unsigned int number)
				{ return get()->get_dev(number); }

			unsigned int register_dev(device * dev);
			void unregister_dev(unsigned int number);
			device * get_dev(unsigned int number) const { return devices[number]; }
		private:
			device_mgr();
			~device_mgr() { delete[] devices; }

			unsigned int next_devnum = 0;

			device ** devices;
			static device_mgr * devmgr;
	};
}

#endif

