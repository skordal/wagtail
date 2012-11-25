// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DEVICE_H
#define WAGTAIL_DEVICE_H

#include "device_mgr.h"
#include "kstream.h"

namespace wagtail
{
	// Base class for all devices:
	class device
	{
		public:
			// Creates a device with the specified name:
			device(const kstring & name) : name(name), number(device_mgr::get()->register_device(this)) {}
			virtual ~device() {}

			const kstring & get_name() const { return name; }
			void set_name(const kstring & name) { this->name = name; }

			void set_number(int number) { this->number = number; }
			int get_number() const { return number; }

			// Methods used in preventing multiple modules from taking control over
			// the same device:
			void reserve() { reserved = true; }
			void release() { reserved = false; }
			bool is_reserved() const { return reserved; }
		private:
			kstring name;
			unsigned int number;
			bool reserved = false;
	};

	// Base class for character devices:
	class character_device : public device, public kostream, public kistream
	{
		public:
			character_device(const kstring & name) : device(name) {}
	};

	// Base class for block devices:
	class block_device : public device
	{
		public:
			// Type representing block addresses for block devices:
			typedef unsigned long long block_address_t;
			static_assert(sizeof(block_address_t) == 8, "block_address_t should be 64 bits long");

			// Constructs a block device object with the specified name and block size:
			block_device(unsigned int block_size, const kstring & name) : device(name), block_size(block_size) {}

			// Gets the block size for this device:
			virtual unsigned int get_block_size() const { return block_size; }

			// Reads a block from the device:
			virtual bool read_block(void * buffer, block_address_t address) = 0;
			// Reads several sequential blocks from the device:
			virtual bool read_blocks(void * buffer, block_address_t address, int length);

			// Writes a block to the device:
			virtual bool write_block(const void * buffer, block_address_t address) = 0;
			// Writes several sequenctial blocks to the device:
			virtual bool write_blocks(const void * buffer, block_address_t address, int length);
		private:
			unsigned int block_size;
	};
}

#endif

