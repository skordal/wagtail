// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KERNEL_H
#define WAGTAIL_KERNEL_H

#include "kstream.h"
#include "kthread.h"
#include "scheduler.h"
#include "syscall.h"
#include "uart.h"

namespace wagtail
{
	/**
	 * Core kernel functions.
	 */
	namespace kernel
	{
		/**
		 * Gets the kernel log stream.
		 * @warning this function cannot be used in static, global constructors,
		 *          because the output device may not have been initialized at
		 *          the point where these constructors are run.
		 */
		kostream & message();

		/**
		 * Kernel entry point. This function is called from assembly code located in
		 * `start.S`.
		 */
		extern "C" void kernel_main() __attribute((noreturn));

		/** Halts the kernel. This function is actually located in `start.S`. */
		extern "C" void halt() __attribute((noreturn));

		/** Kernel panic function, prints a short error message and hangs forever. */
		extern "C" void panic() __attribute((noreturn));

		/**
		 * Thread that initializes the kernel. This thread is spawned at the end of
		 * `kernel_main` to finish initializations that are dependant on the scheduler
		 * being operative.
		 */
		class init_thread : public kthread
		{
			public:
				init_thread() : kthread() {}
				int run() override;
		};

		/**
		 * Wagtail version numbers.
		 */
		namespace version
		{
			/** Major version. */
			const int major = 0;
			/** Minor version. */
			const int minor = 1;
			/** Revision number. */
			const int revision = 0;
		}
	}
}

#endif

