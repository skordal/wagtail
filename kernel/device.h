// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_H
#define WAGTAIL_DEVICE_H

#include "device_mgr.h"
#include "io_operation.h"

namespace wagtail
{
	/**
	 * Base class for all device driver classes.
	 */
	class device
	{
		public:
			/**
			 * Constructs a device with the specified device name.
			 * @param name the name of the device. This is the name to be used in `/dev`.
			 */
			device(const kstring & name) : name(name), number(device_mgr::get()->register_device(this)) {}
			/** Device driver destructor. */
			virtual ~device() {}

			/**
			 * Gets the name of the device.
			 * @return the name of the device.
			 */
			const kstring & get_name() const { return name; }

			/**
			 * Sets the name of the device.
			 * @param name the new name of the device.
			 */
			void set_name(const kstring & name) { this->name = name; }

			/**
			 * Sets the device number.
			 * @param number the device number.
			 */
			void set_number(int number) { this->number = number; }
			/**
			 * Gets the device number.
			 * @return the device number.
			 */
			int get_number() const { return number; }

			/**
			 * Reserves the device. Classes reading or (especially) writing to devices
			 * should check if a device is reserved before attempting to use it.
			 */
			void reserve() { reserved = true; }
			/** Releases the device. It may then be reserved by another class instance. */
			void release() { reserved = false; }
			/**
			 * Checks if the device is reserved.
			 * @return `true` if the device is reserved, `false` otherwise.
			 * */
			bool is_reserved() const { return reserved; }
		private:
			kstring name;
			unsigned int number;
			bool reserved = false;
	};

	/**
	 * Base class for character devices. Character devices are devices that work by reading
	 * and/or writing a single character to or from a hardware unit at a time.
	 */
	class character_device : public device
	{
		public:
			/**
			 * Constructs a character device with the specified name.
			 * @param name the name of the character device. This is the name that
			 *             will be used in `/dev`.
			 */
			character_device(const kstring & name) : device(name) {}

			/**
			 * Reads a number of bytes from the character device.
			 * @param buffer the destination buffer for the read operation.
			 * @param size size of the data to read.
			 * @param flags any flags needed for the transfer. How this field is
			 *              interpreted is completely up to the individual drivers.
			 * @return the number of bytes actually read.
			 */
			virtual unsigned int read(char * buffer, unsigned int size, unsigned int flags = 0) = 0;

			/**
			 * Writes a number of bytes to the character device.
			 * @param buffer the source buffer for the write operation.
			 * @param size the length of the data to write.
			 * @param flags any flags needed for the transfer. How this field is
			 *              interpreted is completely up to the individual drivers.
			 * @return the number of bytes actually written.
			 */
			virtual unsigned int write(const char * buffer, unsigned int size, unsigned int flags = 0) = 0;
	};

	/**
	 * Base class for block devices. Block devices read and write data in blocks of a specified
	 * size.
	 */
	class block_device : public device
	{
		public:
			/** Type representing a 64-bit long byte address. */
			typedef unsigned long long block_address_t;
			static_assert(sizeof(block_address_t) == 8, "block_address_t should be 64 bits long");

			/**
			 * Constructs a block device with the specified block size and name.
			 * @param block_size the size of the blocks of the underlying hardware device.
			 * @param name the name of the device. This is the name used under `/dev`.
			 */
			block_device(unsigned int block_size, const kstring & name) : device(name), block_size(block_size) {}

			/**
			 * Gets the block size of the device.
			 * @return the block size of the device.
			 */
			virtual unsigned int get_block_size() const { return block_size; }

			/** Reads a block from a device. Deprecated in favour of `post_read`. */
			bool read_block(void * buffer, block_address_t address) __attribute((deprecated));
			/** Reads multiple blocks from a device. Deprecated in favour of `post_read`. */
			bool read_blocks(void * buffer, block_address_t address, unsigned int length) __attribute((deprecated));

			/**
			 * Posts a read operation to the device.
			 * @param op the operation to enqueue.
			 * @return the same read operation as in `op`.
			 */
			virtual block_read_operation * post_read(block_read_operation * op) = 0;

			/**
			 * Posts a write operation to the device.
			 * @param op the operation to enqueue.
			 * @return the same write operation as in `op`.
			 */
			virtual block_write_operation * post_write(block_write_operation * op) = 0;
		private:
			unsigned int block_size;
	};
}

#endif

