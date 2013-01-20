// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "register_contents.h"

using namespace wagtail;

register_contents::register_contents()
{
	for(int i = 0; i < 15; ++i)
		r[i] = 0;
	spsr = PSR_T|PROCESSOR_MODE_USR;
	return_address = 0x1004;
}

register_contents::register_contents(const register_contents & copy)
{
	for(int i = 0; i < 15; ++i)
		r[i] = copy.r[i];
	spsr = copy.spsr;
	return_address = copy.return_address;
}

kostream & wagtail::operator<<(kostream & output, const register_contents & registers)
{
	for(int i = 0; i < 15; ++i)
		output << "r" << i << ": " << (void *) registers.r[i] << kstream::newline;
	output << "SPSR: " << (void *) registers.spsr << kstream::newline;
	output << "Return address: " << (void *) registers.return_address << kstream::newline;
	return output;
}


