// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "device.h"

using namespace wagtail;

// Constructs a new device node with the specified parameters:
device::device(unsigned char major, unsigned char minor, const char * description)
	: major(major), minor(minor), description(description)
{
	
}

// Compares two devices by comparing their major and minor device numbers:
bool device::operator==(const device & dev)
{
	return dev.get_major() == major && dev.get_minor() == minor;
}

// Prints device information to the specified stream:
kostream & wagtail::operator<<(kostream & stream, const device & dev)
{
	stream << dev.get_major() << ':' << dev.get_minor() << " " << dev.get_description();
	return stream;
}

// Constructs a new block device with the specified parameters:
block_device::block_device(unsigned char major, unsigned char minor, unsigned int block_size,
	const char * description) : device(major, minor, description), block_size(block_size)
{

}

