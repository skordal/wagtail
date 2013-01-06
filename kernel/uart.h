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
	/**
	 * UART device driver.
	 */
	class uart final : public character_device
	{
		public:
			/**
			 * Gets a reference to the driver for a specified UART module.
			 * @param module the module to get the driver for, [0 .. 3].
			 * @return a reference to the driver instance.
			 */
			static uart & get(int module) { return uart_modules[module]; }

			/**
			 * Writes a character to the UART.
			 * @param character the character to write.
			 */
			kostream & operator << (char character) override;
			/**
			 * Reads a character from the UART.
			 * @param character a reference to the variable to store the read character in.
			 */
			kistream & operator >> (char & character) override;
		private:
			/**
			 * Constructs a UART driver object for the specified UART module.
			 * @param module module number
			 * @param device_name name of the device node.
			 */
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

