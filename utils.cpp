// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "utils.h"

using namespace wagtail;

// Masks the bits to the left of and including the bit set in bit:
unsigned int utils::mask_left(int bit)
{
	unsigned int retval;

	asm volatile(
		"clz v1, %[bit]\n\t"
		"mvn v2, #0\n\t"
		"adds v1, #1\n\t"
		"lsrs v2, v1\n\t"
		"mvns %[retval], v2\n\t"
		: [retval] "=r" (retval)
		: [bit] "r" (bit)
		: "v1", "v2", "cc"
	);

	return retval;
}

