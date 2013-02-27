// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SD_H
#define WAGTAIL_SD_H

#include <functional>

#include "clock_mgr.h"
#include "device.h"
#include "io_operation.h"
#include "mmu.h"
#include "partition.h"

#include "hardware/sd.h"

// Define to enable SD card debugging messages, which include printing the
// commands being sent to the card:
//#define WAGTAIL_SD_DEBUG

namespace wagtail
{
	/**
	 * SD card driver class.
	 * @todo Use DMA and interrupts for I/O.
	 */
	class sd final : public block_device
	{
		public:
			/** Initializes the SD card module instance. */
			static void initialize();

			/**
			 * Posts a read operation to the SD card.
			 * @param read_op the read operation object.
			 */
			block_read_operation * post_read(block_read_operation * read_op) override;

			/**
			 * Posts a write operation to the SD card.
			 * @param write_op the write opearation object.
			 */
			block_write_operation * post_write(block_write_operation * write_op) override;

			/**
			 * SD card module base address.
			 * There is only one SD/MMC controller in use on the BeagleBoard, so the SD
			 * driver can be a singleton for now.
			 */
			static constexpr void * BASE_ADDRESS = reinterpret_cast<void *>(0x4809c000);
		private:
			/** Initializes the SD card module. */
			sd();

			/** Initializes a newly connected card. */
			void initialize_card();

			/** Reads the partition table of a connected and initialized card. */
			void read_partition_table();

			/** Switches the SD bus clock divider. */
			void switch_clock_divider(short divider);

			// Various functions for sending commands to the card, used during
			// the card initialization procedure:
			void send_cmd0();	// GO_IDLE_STATE
			bool send_cmd2();	// ALL_SEND_CID
			bool send_cmd3();	// SEND_RELATIVE_ADDR
			bool send_cmd7();	// SELECT/DESELECT_CARD
			bool send_cmd8();	// SEND_IF_COND
			bool send_cmd55();	// APP_CMD
			bool send_acmd6();	// SET_BUS_WIDTH
			int send_acmd41();	// SD_SEND_OP_COND

			// More functions for sending commands to the card, used during
			// normal operation:
			void send_cmd17(block_address_t address);	// READ_SINGLE_BLOCK

			/** Class representing the bus status after a command has been sent. */
			enum class sd_status
			{
				card_error,
				data_timeout_error, data_end_bit_error,
				data_crc_error, command_end_bit_error,
				command_crc_error, command_index_error,
				command_timeout, command_complete
			};

			/**
			 * Waits until there is any indication in the status register.
			 * @return the status retrieved from the status register. 
			 */
			sd_status wait_for_status();

			/**
			 * Gets an error message corresponding to a status code.
			 * @param status the status to look up.
			 * @return an error message for the specified status code.
			 */
			const char * get_error_message(sd_status status) const;

			/**
			 * Initiates a read operation.
			 * @param read_op the read operation to initiate.
			 */
			void initiate_read(block_read_operation * read_op);

			/**
			 * Starts the next read operation in the queue.
			 */
			void initiate_next_read();

			/**
			 * Handles an SD card module IRQ interrupt.
			 * @param irq interrupt number.
			 */
			void irq_handler(int irq);

			unsigned short rca = 0x0000;
			partition * partitions[4] = {nullptr, nullptr, nullptr, nullptr};

			block_read_operation * active_read = nullptr;
			int read_attempts = 0;
			kqueue<block_read_operation *> read_queue;

			void * virtual_base;
			static sd * sd_module;
	};
}

#endif

