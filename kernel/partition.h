// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PARTITION_H
#define WAGTAIL_PARTITION_H

#include "device.h"
#include "device_mgr.h"
#include "utils.h"

namespace wagtail
{
	class partition : public block_device
	{
		public:
			partition(block_device * device, block_address_t start_addr,
				block_address_t end_addr, char type, int num);
			~partition() { delete[] name; }

			bool read_block(void * buffer, block_address_t address) override
				{ return device->read_block(buffer, address); }
			bool read_blocks(void * buffer, block_address_t address, int length) override
				{ return device->read_blocks(buffer, address, length); }

			bool write_block(const void * buffer, block_address_t address) override
				{ return device->write_block(buffer, address); }
			bool write_blocks(const void * buffer, block_address_t address, int length) override
				{ return device->write_blocks(buffer, address, length); }

			char get_type() const { return type; }
		private:
			// Creates a partition device name from the basename of
			// the parent block drive and the partition number, looking
			// like, for example, for SD cards, sd0p[0,1,2,3].
			static char * partname(const char * base, int partnum);

			block_device * device;
			block_device::block_address_t start, end;

			char type;
			char * name;
	};
}

#endif

