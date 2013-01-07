// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_IRQ_HANDLER_H
#define WAGTAIL_IRQ_HANDLER_H

#include <functional>

#include "io.h"
#include "mmu.h"

#include "hardware/intc.h"

/**
 * Handles an IRQ interrupt. This method is called from the interrupt handler in
 * `interrupts.S`.
 */
extern "C" void handle_irq();

namespace wagtail
{
	/**
	 * IRQ handler class. This class facilitates registering and unregistering of IRQ handlers,
	 * as well as calling these when an interrupt occurs.
	 */
	class irq_handler final
	{
		public:
			/** Initializes the global IRQ handler instance. */
			static void initialize();
			/**
			 * Gets the global IRQ handler instance.
			 * @return the global IRQ handler instance.
			 */
			static irq_handler * get() { return global_irq_handler; }

			/**
			 * Registers an IRQ handler. The corresponding IRQ interrupt is also unmasked
			 * in the interrupt controller.
			 * @param handler the interrupt handler function.
			 * @param number the IRQ number to register the handler for.
			 * @param priority the priority to assign to the IRQ.
			 */
			void register_handler(std::function<void(int)> handler, int number, int priority = PRIORITY_MID);
			/**
			 * Unregisters an IRQ handler. The corresponding IRQ interrupt is masked and
			 * disabled in the interrupt controller.
			 * @param number the IRQ number to remove the handler for.
			 */
			void unregister_handler(int number);

			/** The lowest IRQ priority. */
			static const int PRIORITY_LOW   = 63;
			/** Medium (normal) IRQ priority. */
			static const int PRIORITY_MID   = 31;
			/** The highest IRQ priority. */
			static const int PRIORITY_HIGH  =  0;
		private:
			static const int NUMBER_OF_IRQS = 96;

			static const int THRESHOLD_DISABLED = 0xff;
			static const int DEFAULT_THRESHOLD  = THRESHOLD_DISABLED;

			irq_handler();
			void handle_irq(int number);

			void * virtual_base, * irq_stack;
			std::function<void(int)> irq_handlers[NUMBER_OF_IRQS];

			static irq_handler * global_irq_handler;

			friend void ::handle_irq();
	};
}

#endif

