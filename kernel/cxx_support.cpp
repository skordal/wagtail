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

// Function required for copying memory in copy constructors:
extern "C" void * memcpy(void * dest, void * src, unsigned int n)
{
	for(unsigned int i = 0; i < n; ++i)
		*((char *) dest + i) = *((char *) src + i);
	return dest;
}

// Function required for setting memory in constructors:
extern "C" void * memset(void * d, char c, unsigned int n)
{
	for(unsigned int i = 0; i < n; ++i)
		*((char *) d + i) = c;
	return d;
}

// Function required to use std::function objects without checking if they are nullptr:
void std::__throw_bad_function_call()
{
	kernel::message() << kstream::newline
		<< "*** FATAL ERROR: A callback function call failed! ***" << kstream::newline
		<< "*** FATAL ERROR: This is most likely caused by calling a nullptr function pointer." << kstream::newline;
	kernel::panic();
}

