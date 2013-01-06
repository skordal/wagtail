// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KERNEL_H
#define WAGTAIL_KERNEL_H

#include "filesystem.h"
#include "irq_handler.h"
#include "kstream.h"
#include "mm.h"
#include "partition.h"
#include "scm.h"
#include "sd.h"
#include "syscall.h"
#include "uart.h"
#include "vfs.h"

namespace wagtail
{
	/**
	 * Central kernel methods.
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
		 * Kernel entry point. This method is called from assembly code located in
		 * `start.S`.
		 */
		extern "C" void kernel_main();

		/**
		 * Kernel panic function, prints a short error message and hangs forever.
		 */
		extern "C" void panic() __attribute((noreturn));

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

