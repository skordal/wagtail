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
	/**
	 * Class representing a partition on a block device.
	 * It also works as a wrapper for the block device itself, where reads and
	 * writes are bounded to the partition limits.
	 */
	class partition final : public block_device
	{
		public:
			/** Partition type signifying that the filesystem type should be automatically detected. */
			static const char auto_type = -1;

			/**
			 * Constructs a partition object.
			 * @param device the underlying block device.
			 * @param start_addr start address of the partition.
			 * @param end_addr end address of the partition.
			 * @param type MBR partition type code.
			 * @param num partition number on the device.
			 */
			partition(block_device * device, block_address_t start_addr,
				block_address_t end_addr, char type, int num);
			/** Destructs a partition object. */
			virtual ~partition() {}

			block_read_operation * post_read(block_read_operation * read_op) override;
			block_write_operation * post_write(block_write_operation * write_op) override;

			/**
			 * Gets the start address of the partition.
			 * @return the start address of the partition.
			 */
			block_address_t get_start_address() const { return start; }
			/**
			 * Gets the end address of the partition.
			 * @return the end address of the partition.
			 */
			block_address_t get_end_address() const { return end; }

			/**
			 * Gets the partition type code.
			 * @return the partition type code.
			 */
			char get_type() const { return type; }

			/**
			 * Gets the partition size.
			 * @return the partition size, in blocks.
			 */
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

