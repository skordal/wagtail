// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SD_CARD_H
#define WAGTAIL_SD_CARD_H

#include "device.h"
#include "io.h"
#include "kernel.h"
#include "mmu.h"

namespace wagtail
{
	// SD card driver. There is only one connected SD card interface on
	// the BeagleBoard, so for now we assume there is only one interface
	// in total, for ease of programming.
	class sd_card : public block_device
	{
		public:
			// Initializes the SD card interface:
			static void initialize();

			// Gets the number of partitions on the SD card:
			int get_num_partitions() const;

			// Reads a block from the SD card:
			void read_block(void * buffer, block_address_t address);
			// Writes a block to the SD card:
			void write_block(void * buffer, block_address_t address);
		private:
			// SD module register data:
			struct registers
			{
				struct sysconfig {
					static const unsigned int offset = 0x10;

					// Valid values for the idlemode bitfield:
					enum idlemode_t
					{
						idlemode_auto		= 0x2,
						idlemode_ignore		= 0x1,
						idlemode_unconditional	= 0x0
					};
					
					// Bit offsets:
					static inline const int sidlemode(idlemode_t x) { return x << 3; }
					static const int enawakeup	= 1 << 2;
					static const int softreset	= 1 << 1;
					static const int autoidle	= 1 << 0;
				};

				struct sysstatus {
					static const unsigned int offset = 0x14;

					static const int resetdone = 1 << 0;
				};
			};

			sd_card();

			// The SD card physical base address:
			static void * base_address;

			// The SD card virtual base address:
			void * virtual_base;

			// The SD kernel card object:
			static sd_card * sd;
	};
}

#endif

