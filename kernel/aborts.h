// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_ABORTS_H
#define WAGTAIL_ABORTS_H

#include "kernel.h"

namespace wagtail
{
	// Called to handle a data abort exception:
	extern "C" void data_abort();
	// Called to handle a prefetch abort exception:
	extern "C" void prefetch_abort(); // TODO: Write me
}

#endif

