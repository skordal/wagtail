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

unsigned int utils::strlen(const char * string)
{
	unsigned int count = 0;
	for(; string[count] != 0; ++count);
	return count;
}

char * utils::strcpy(char * dest, const char * source)
{
	for(unsigned int i = 0; i < strlen(source) + 1; ++i)
		dest[i] = source[i];
	return dest;
}

char * utils::strcat(char * dest, const char * source)
{
	unsigned int offset = strlen(dest);
	for(unsigned int i = 0; i < strlen(source) + 1; ++i)
		dest[offset + i] = source[i];

	return dest;
}

bool utils::str_equals(const char * a, const char * b)
{
	if(strlen(a) != strlen(b))
		return false;
	else {
		for(unsigned int i = 0; i < strlen(a); ++i)
			if(a[i] != b[i])
				return false;
		return true;
	}
}

