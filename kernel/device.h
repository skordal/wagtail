// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_H
#define WAGTAIL_DEVICE_H

#include "device_mgr.h"
#include "kstream.h"

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
	class character_device : public device, public kostream, public kistream
	{
		public:
			/**
			 * Constructs a character device with the specified name.
			 * @param name the name of the character device. This is the name that
			 *             will be used in `/dev`.
			 */
			character_device(const kstring & name) : device(name) {}
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

			/**
			 * Reads a block from the device.
			 * @param buffer the target buffer.
			 * @param address the address to read from.
			 * @return `true` if successful, `false` otherwise.
			 */
			virtual bool read_block(void * buffer, block_address_t address) = 0;
			/**
			 * Reads several sequential blocks from the device.
			 * @param buffer the target buffer.
			 * @param address the address to read from.
			 * @param length the number of blocks to read.
			 * @return `true` if successful, `false` otherwise.
			 */
			virtual bool read_blocks(void * buffer, block_address_t address, int length);

			/**
			 * Writes a block to the device.
			 * @param buffer the buffer to read data from.
			 * @param address the address to write the data to.
			 * @return `true` if successul, `false` otherwise.
			 */
			virtual bool write_block(const void * buffer, block_address_t address) = 0;
			/**
			 * Writes several sequential blocks to the device.
			 * @param buffer the buffer to read data from.
			 * @param address the address to write the data to.
			 * @param length the number of blocks to write.
			 */
			virtual bool write_blocks(const void * buffer, block_address_t address, int length);
		private:
			unsigned int block_size;
	};
}

#endif

