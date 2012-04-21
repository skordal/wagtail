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

// Reads a set of blocks from the block device:
void block_device::read_blocks(void * buffer, block_address_t base_address, int length)
{
	for(int i = 0; i < length; ++i)
		read_block(reinterpret_cast<void *>((unsigned int) buffer + (i * block_size)),
			base_address + (i * block_size));
}

// Writes a set of blocks to the block device:
void block_device::write_blocks(const void * buffer, block_address_t base_address, int length)
{
	for(int i = 0; i < length; ++i)
		write_block(reinterpret_cast<void *>((unsigned int) buffer + (i * block_size)),
			base_address + (i * block_size));
}

