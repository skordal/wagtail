// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SD_H
#define WAGTAIL_SD_H

#include "clock_mgr.h"
#include "device.h"
#include "mmu.h"

#include "hardware/sd.h"

namespace wagtail
{
	class sd : public block_device
	{
		public:
			// Initializes the SD module instance:
			static void initialize();

			// TODO: Implement read and write functions.
			void read_block(void * buffer, block_address_t address) {}
			void write_block(void * buffer, block_address_t address) {}

			static constexpr void * BASE_ADDRESS = reinterpret_cast<void *>(0x4809c000);
		private:
			// Initializes the SD controller:
			sd();

			// Initializes a newly connected card:
			void initialize_card();

			// Various functions for sending commands to the card:
			void send_cmd0();
			bool send_cmd2();
			bool send_cmd3();
			bool send_cmd8();
			bool send_cmd55();
			int send_acmd41(); // Returns -1 on error, otherwise returns the command response.

			enum class sd_status
			{
				card_error,
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

			void * virtual_base;
			static sd * sd_module;
	};
}

#endif

