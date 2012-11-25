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
	// The partition class represents a filesystem partition on a block device.
	// It also works as a wrapper for the block device itself.
	class partition : public block_device
	{
		public:
			static const char auto_type = -1;

			partition(block_device * device, block_address_t start_addr,
				block_address_t end_addr, char type, int num);
			~partition() {}

			// Overrides for doing I/O on the underlying block device; these methods
			// checks if the specified block address is within the bounds of the partition,
			// and only allows I/O inside it.
			bool read_block(void * buffer, block_address_t address) override;
			bool read_blocks(void * buffer, block_address_t address, int length) override;
			bool write_block(const void * buffer, block_address_t address) override;
			bool write_blocks(const void * buffer, block_address_t address, int length) override;

			block_address_t get_start_address() const { return start; }
			block_address_t get_end_address() const { return end; }

			// Gets the partition type:
			char get_type() const { return type; }

			unsigned long long get_size() const { return end - start; }
		private:
			// Creates a partition device name from the basename of
			// the parent block drive and the partition number, looking
			// like, for example, for SD cards, sd0p[0,1,2,3].
			static kstring partname(const kstring & base, int partnum);

			block_device * device;
			block_device::block_address_t start, end;

			char type;
	};
}

#endif

