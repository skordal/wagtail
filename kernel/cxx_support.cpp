// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "cxx_support.h"

using namespace wagtail;

// Function required for using pure virtual classes:
extern "C" void __cxa_pure_virtual()
{
	kernel::message() << kstream::newline
		<< "*** FATAL ERROR: A pure virtual function call failed! ***" << kstream::newline
		<< "*** FATAL ERROR: Your kernel binary is probably erroneously compiled and linked." << kstream::newline;
	kernel::panic();
}

