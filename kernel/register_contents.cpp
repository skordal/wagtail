// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "register_contents.h"

using namespace wagtail;

kostream & wagtail::operator<<(kostream & output, const register_contents & registers)
{
	for(int i = 0; i < 15; ++i)
		output << "r" << i << ": " << (void *) registers.r[i] << kstream::newline;
	output << "SPSR: " << (void *) registers.spsr << kstream::newline;
	output << "Return address: " << (void *) registers.return_address << kstream::newline;
	return output;
}


