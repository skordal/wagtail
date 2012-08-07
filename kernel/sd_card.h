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
			// Initializes the connected SD card:
			void initialize_card();

			// Methods for sending commands to the card:
			bool send_cmd0();	// GO_IDLE_STATE
			bool send_cmd2();	// ALL_SEND_CID
			bool send_cmd3();	// SEND_RELATIVE_ADDR
			bool send_cmd8();	// SEND_IF_COND
			bool send_cmd55();	// APP_CMD

			// Methods for sending application commands to the card:
			bool send_acmd41();

			// Waits for a command to finish sending; returns false if
			// an error occurs and prints an error message to the kernel
			// log:
			bool wait_for_cmd_sent();

			// Card RCA:
			unsigned short rca;

			// Constructor, does most of the initialization:
			sd_card();

			// The SD card virtual base address:
			void * virtual_base;

			// The SD kernel card object:
			static sd_card * sd;

			// SD module register definitions:
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
					static constexpr int sidlemode(idlemode_t x) { return x << 3; }

					static const int enawakeup	= 1 << 2;
					static const int softreset	= 1 << 1;
					static const int autoidle	= 1 << 0;
				};

				struct sysstatus {
					static const unsigned int offset = 0x14;
					static const int resetdone = 1 << 0;
				};

				struct con {
					static const unsigned int offset = 0x2c;

					static const int init	= 1 << 1;
					static const int od	= 1 << 0;
				};

				struct arg {
					static const unsigned int offset = 0x108;
				};

				struct rsp10 {
					static const unsigned int offset = 0x110;
				};

				struct cmd {
					static const unsigned int offset = 0x10c;

					enum response_type_t
					{
						r1 = 0x2,
						r1b = 0x3,
						r2 = 0x1,
						r3 = 0x2,
						r6 = 0x2,
						r7 = 0x2
					};
					static constexpr int rsp_type(response_type_t x) { return x << 16; }
					static constexpr int indx(int x) { return x << 24; }
				};

				struct hctl {
					static const unsigned int offset = 0x128;
					
					enum sd_bus_voltage_t
					{
						sd_bus_voltage_18	= 0x5,
						sd_bus_voltage_30	= 0x6,
						sd_bus_voltage_33	= 0x7
					};
					static constexpr int sdvs(sd_bus_voltage_t x) { return x << 9; }

					static const int sdbp	= 1 << 8;
					static const int dtw	= 1 << 1;
				};

				struct sysctl {
					static const unsigned int offset = 0x12c;

					static const int src = 1 << 25;
					static const int sra = 1 << 24;
					static constexpr int clkd(unsigned int x) { return (x & 0x3ff) << 6; }
					static const int cen = 1 << 2;
					static const int ics = 1 << 1;
					static const int ice = 1 << 0;

					static const int clkd_mask = 0x3ff << 6;
				};

				struct stat {
					static const unsigned int offset = 0x130;
					static const int cerr	= 1 << 28;
					static const int cto	= 1 << 16;
					static const int cc	= 1 <<  0;
				};

				struct ie {
					static const unsigned int offset = 0x134;

					static const int bada_enable	= 1 << 29;
					static const int cerr_enable	= 1 << 28;
					static const int ace_enable	= 1 << 24;
					static const int deb_enable	= 1 << 22;
					static const int dcrc_enable	= 1 << 21;
					static const int dto_enable	= 1 << 20;
					static const int cie_enable	= 1 << 19;
					static const int ceb_enable	= 1 << 18;
					static const int ccrc_enable	= 1 << 17;
					static const int cto_enable	= 1 << 16;
					static const int obi_enable	= 1 <<  9;
					static const int cirq_enable	= 1 <<  8;
					static const int brr_enable	= 1 <<  5;
					static const int bwr_enable	= 1 <<  4;
					static const int bge_enable	= 1 <<  2;
					static const int tc_enable	= 1 <<  1;
					static const int cc_enable	= 1 <<  0;
				};

				struct capa {
					static const unsigned int offset = 0x140;

					static const int vs18 = 1 << 26;
					static const int vs30 = 1 << 25;
					static const int vs33 = 1 << 24;
				};

				struct cur_capa {
					static const unsigned int offset = 0x148;
				};
			};

	};
}

#endif

