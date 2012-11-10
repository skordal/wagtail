// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_UART_H
#define WAGTAIL_UART_H

#include "device.h"
#include "io.h"
#include "mmu.h"

#include "hardware/uart.h"

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
			uart(int module, const char * device_name);

			// The number of this module:
			int module;

			// UART module virtual base addresses:
			static void * uart_base[4];

			// UART objects corresponding to the 4 UART modules:
			static uart uart_modules[4];
	};
}

#endif

