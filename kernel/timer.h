// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_TIMER_H
#define WAGTAIL_TIMER_H

#include <functional>

#include "device.h"
#include "io.h"
#include "irq_handler.h"
#include "mmu.h"

#include "hardware/timer.h"

namespace wagtail
{
	/**
	 * Class representing a hardware timer.
	 */
	class timer final : public device
	{
		public:
			/** Possible timer modes. Disabled is the default state. */
			enum class mode
			{
				disabled, /**< The timer is disabled. */
				one_shot, /**< The timer fires once and is then disabled. */
				repeating /**< The timer fires repeatedly. */
			};

			/**
			 * Gets a reference to the specified timer module driver.
			 * @param module the module to get, in [0 .. 10].
			 * @return a reference to the specified timer module driver.
			 */
			static timer & get(int module) { return timer_modules[module]; }

			/**
			 * Starts the timer.
			 * @param callback the function to call when the timer fires.
			 * @param mode the timer mode.
			 * @param interval the timer interval in µs.
			 */
			void start(std::function<void()> callback, timer::mode mode, unsigned int interval);

			/** Stops the timer. */
			void stop();
		private:
			/** IRQ of the first timer module. */
			static const int IRQ_BASE = 37;

			/**
			 * Sets up the timer driver for the specified module.
			 * @param n the timer module number, in [0 .. 10].
			 * @param name the name of the timer device node.
			 */
			timer(int n, const char * name);

			/**
			 * Handles a timer IRQ.
			 * @param n the IRQ number.
			 */
			static void irq_handler(int n);

			// Number of the module to represent:
			int module;
			// Timer mode:
			mode timer_mode = mode::disabled;

			std::function<void()> callback = nullptr;
			unsigned int interval = 0;

			static void * timer_base[11];
			static timer timer_modules[11];
	};
}

#endif

