// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_H
#define WAGTAIL_DEVICE_H

#include "kstream.h"

namespace wagtail
{
	// Device major numbers:
	namespace device_numbers
	{
		const int uart_major = 1;
	}

	// Base class for all devices:
	class device
	{
		public:
			// Creates a device with the specified major and minor
			// numbers. A device description should also be provided:
			device(unsigned char major, unsigned char minor,
				const char * description);

			unsigned char get_major() const { return major; }
			unsigned char get_minor() const { return minor; }
			const char * get_description() const { return description; }

			// Compares two device classes by comparing their major and
			// minor device numbers:
			bool operator==(const device & dev);
		private:
			unsigned char major, minor;
			const char * description;
	};

	// Prints device information to the specified stream:
	kostream & operator<<(kostream & stream, const device & dev);

	// Base class for character devices:
	class character_device : public device, public kostream, public kistream
	{
		public:
			character_device(unsigned char major, unsigned char minor, const char * description)
				: device(major, minor, description) {}
	};

	// Base class for block devices:
	class block_device : public device
	{
		public:
			block_device(unsigned char major, unsigned char minor,
				unsigned int block_size, const char * description);

			unsigned int get_block_size() const { return block_size; }
		private:
			unsigned int block_size;
	};
}

#endif

