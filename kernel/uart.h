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
			// UART module virtual base addresses:
			static void * uart_base[4];

			// Constructs a UART object for the specified UART module:
			uart(int module);

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

				struct lsr
				{
					static const unsigned int offset = 0x14;

					static const int tx_fifo_e = 1 << 5;
					static const int rx_fifo_e = 1 << 0;
				};
			};

			// The number of this module:
			int module;

			// UART objects corresponding to the 4 UART modules:
			static uart uart_modules[4];
	};
}

#endif

