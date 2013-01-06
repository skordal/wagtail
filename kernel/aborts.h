// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_ABORTS_H
#define WAGTAIL_ABORTS_H

#include "kernel.h"

namespace wagtail
{
	/**
	 * Handles a data abort. This method is called from the data abort interrupt
	 * handler in `interrupts.S`.
	 */
	extern "C" void data_abort();

	/**
	 * Handles a prefetch abort. This method is called from the data abort interrupt
	 * handler in `interrupts.S`.
	 */
	extern "C" void prefetch_abort();
}

#endif

