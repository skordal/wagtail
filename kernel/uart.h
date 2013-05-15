// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_UART_H
#define WAGTAIL_UART_H

#include <functional>

#include "io.h"
#include "irq_handler.h"
#include "mmu.h"

#include "hardware/uart.h"

namespace wagtail
{
	/**
	 * UART device driver.
	 */
	class uart final : public kostream
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
			 * Reads data from the UART.
			 * @warning This function is unimplemented at the moment. The UART is currently
			 *          set to echo any input given to it.
			 * @param buffer the destination buffer.
			 * @param size the size of the data to read.
			 * @param flags unused parameter.
			 * @return the number of bytes actually read from the UART.
			 */
			unsigned int read(char * buffer, unsigned int size, unsigned int flags = 0) { return 0; }

			/**
			 * Writes data to the UART.
			 * @param buffer the source buffer.
			 * @param size the size of the data to write.
			 * @param flags unused parameter.
			 * @return the number of bytes written to the UART.
			 */
			unsigned int write(const char * buffer, unsigned int size, unsigned int flags = 0);
		private:
			/** Base of the UART IRQ numbers. */
			static const int IRQ_BASE = 72;

			/**
			 * UART IRQ handler.
			 * @param irq number of the IRQ that caused the handler to be called.
			 */
			void interrupt_handler(int irq);

			/**
			 * Constructs a UART driver object for the specified UART module.
			 * @param module module number
			 */
			uart(int module);

			// The number of this module:
			int module;

			// UART module virtual base addresses:
			static void * uart_base[4];

			// UART objects corresponding to the 4 UART modules:
			static uart uart_modules[4];
	};
}

#endif

