// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_IRQ_HANDLER_H
#define WAGTAIL_IRQ_HANDLER_H

#include "io.h"
#include "mmu.h"

#include "hardware/intc.h"

extern "C" void handle_irq();
extern "C" void test_irq(int number, void * data);

namespace wagtail
{
	class irq_handler
	{
		public:
			typedef struct
			{
				void (* handler_func)(int number, void * data);
				void * data;
			} irq_handler_t;

			static void initialize();
			static irq_handler * get() { return global_irq_handler; }

			// Registers an IRQ handler and enables the interrupt:
			void register_handler(const irq_handler_t & handler, int number, int priority = PRIORITY_MID);
			// Unregisters and disables an IRQ interrupt handler:
			void unregister_handler(int number);

			static const int NUMBER_OF_IRQS = 96;
			static const int PRIORITY_LOW   = 63;
			static const int PRIORITY_MID   = 31;
			static const int PRIORITY_HIGH  =  0;

			static const int THRESHOLD_DISABLED = 0xff;
			static const int DEFAULT_THRESHOLD  = THRESHOLD_DISABLED;
		private:
			irq_handler();

			void handle_irq(int number);

			void * virtual_base;
			irq_handler_t * irq_handlers[NUMBER_OF_IRQS];

			static irq_handler * global_irq_handler;

			friend void ::handle_irq();
	};
}

#endif

