// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "device.h"

using namespace wagtail;

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

