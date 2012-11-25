// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_UTILS_H
#define WAGTAIL_UTILS_H

namespace wagtail
{
	namespace utils
	{
		// Masks the bits to the left of and including the bit set in bit:
		unsigned int mask_left(int bit);

		// Calculates the length of a C string:
		unsigned int strlen(const char * string);

		// Duplicates a C string, equal to creating a new buffer and using strcpy:
		char * strdup(const char * source);

		// Copies a C string into another buffer:
		char * strcpy(char * dest, const char * source);

		// Appends a C string onto another C string:
		char * strcat(char * dest, const char * source);
	}
}

#endif

