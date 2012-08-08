// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_UART_H
#define WAGTAIL_UART_H

#include "device.h"
#include "io.h"
#include "mmu.h"

namespace wagtail
{
	class uart : public character_device
	{
		public:
			// Gets the uart object for the specified UART module:
			static uart & get(int module) { return uart_modules[module]; }

			// Writes a character to an UART module:
			kostream & operator << (char character);
			// Reads a character from a UART module:
			kistream & operator >> (char & character);

		private:
			// Constructs a UART object for the specified UART module:
			uart(int module);

			// The number of this module:
			int module;

			// UART module virtual base addresses:
			static void * uart_base[4];

			// UART objects corresponding to the 4 UART modules:
			static uart uart_modules[4];

			// UART register data:
			struct registers
			{
				struct thr
				{
					static const unsigned int offset = 0x00;
				};

				struct rhr
				{
					static const unsigned int offset = 0x00;
				};

				struct dll
				{
					static const unsigned int offset = 0x00;
				};

				struct dlh
				{
					static const unsigned int offset = 0x04;
				};

				struct efr
				{
					static const unsigned int offset = 0x08;
					static const char enhanced_en = 1 << 4;
				};

				struct fcr
				{
					static const unsigned int offset = 0x08;
					static const char fifo_en = 1 << 0;
				};

				struct lcr
				{
					static const unsigned int offset = 0x0c;
					static const int parity_type1	= 1 << 4;
					static const int parity_en	= 1 << 3;
				};

				struct lsr
				{
					static const unsigned int offset = 0x14;

					static const char tx_fifo_e = 1 << 5;
					static const char rx_fifo_e = 1 << 0;
				};

				struct mdr1
				{
					static const unsigned int offset = 0x20;
				};

				struct ssr
				{
					static const unsigned int offset = 0x44;
					static const char tx_fifo_full = 1 << 0;
				};

				struct sysc
				{
					static const unsigned int offset = 0x54;

					enum idlemode_t
					{
						force = 0,
						no = 1,
						smart = 2
					};

					static constexpr int idlemode(idlemode_t x) { return x << 3; }
					static const char softreset	= 1 << 1;
					static const char autoidle	= 1 << 0;
				};
			};
	};
}

#endif

