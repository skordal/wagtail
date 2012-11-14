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
#include "uart.h"

namespace wagtail
{
	namespace kernel
	{
		// Returns a stream for printing a message to the kernel log.
		// NOTE: this function cannot be used in global static constructors,
		//       because the output device may not have been initialized at
		//       the point that the global static constructors are run.
		kostream & message();

		// Kernel entry point, called from assembler code:
		extern "C" void kernel_main();

		// Kernel panic function, stops everything and prints a short error message:
		extern "C" void panic() __attribute((noreturn));

		// Wagtail version number:
		namespace version
		{
			const int major = 0;
			const int minor = 1;
		}
	}
}

#endif

