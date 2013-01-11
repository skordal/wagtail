// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SCHEDULER_H
#define WAGTAIL_SCHEDULER_H

#include <functional>

#include "file.h"
#include "process.h"
#include "register_contents.h"
#include "timer.h"
#include "vfs.h"

namespace wagtail
{
	/**
	 * The scheduler class. The scheduler takes care of switching between
	 * the active user-mode processes. It uses `timer0` to generate a regular
	 * interrupt for the context switching.
	 */
	class scheduler final
	{
		public:
			/** Initializes the scheduler. */
			static void initialize() { kernel_scheduler = new scheduler; }
			/**
			 * Gets the scheduler instance.
			 * @return the global scheduler instance.
			 */
			static scheduler * get() { return kernel_scheduler; }

			/** Starts the scheduler. */
			void start();
		private:
			scheduler();

			/**  Scheduler interrupt handler. */
			void interrupt();

			bool running = true;
			process * test_process;
			static scheduler * kernel_scheduler;
	};
}

#endif

