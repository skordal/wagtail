// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SD_H
#define WAGTAIL_SD_H

#include "clock_mgr.h"
#include "device.h"
#include "mmu.h"
#include "partition.h"

#include "hardware/sd.h"

// Define to enable SD card debugging messages, which include printing the
// commands being sent to the card:
// #define WAGTAIL_SD_DEBUG

namespace wagtail
{
	// SD card driver class.
	// TODO: Use DMA for the reading and writing functionality.
	class sd final : public block_device
	{
		public:
			// Initializes the SD module instance:
			static void initialize();

			// Reads a block of data from the SD card:
			bool read_block(void * buffer, block_address_t address) override;
			// Writes a block of data to the SD card:
			bool write_block(const void * buffer, block_address_t address) override;

			// Only one SD/MMC controller is in use on the BeagleBoard, so only that
			// controller is used in Wagtail.
			static constexpr void * BASE_ADDRESS = reinterpret_cast<void *>(0x4809c000);
		private:
			// Initializes the SD controller:
			sd();

			// Initializes a newly connected card:
			void initialize_card();

			// Reads the partition table of an initialized card:
			void read_partition_table();

			// Switches the SD bus frequency clock divider:
			void switch_clock_divider(short divider);

			// Various functions for sending commands to the card:
			void send_cmd0();	// GO_IDLE_STATE
			bool send_cmd2();	// ALL_SEND_CID
			bool send_cmd3();	// SEND_RELATIVE_ADDR
			bool send_cmd7();	// SELECT/DESELECT_CARD
			bool send_cmd8();	// SEND_IF_COND
			bool send_cmd55();	// APP_CMD
			bool send_acmd6();	// SET_BUS_WIDTH
			int send_acmd41();	// SD_SEND_OP_COND

			enum class sd_status
			{
				card_error,
				data_timeout_error, data_end_bit_error,
				data_crc_error, command_end_bit_error,
				command_crc_error, command_index_error,
				command_timeout, command_complete
			};

			// Waits until there is an indication in the status register,
			// used when sending commands:
			sd_status wait_for_status();

			// Gets an error message for the specified SD status:
			const char * get_error_message(sd_status status) const;

			bool card_initialized = false;
			unsigned short rca = 0x0000;
			partition * partitions[4] = {nullptr, nullptr, nullptr, nullptr};

			void * virtual_base;
			static sd * sd_module;
	};
}

#endif

