// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_UTILS_H
#define WAGTAIL_UTILS_H

namespace wagtail
{
	/**
	 * Various utility functions.
	 */
	namespace utils
	{
		/**
		 * Masks the bits to the left of and including the rightmost bit specified.
		 * @param bit the bit value to mask left of.
		 * @return a bitmask of the bits to the left of and including the rightmost bit
		 *         specified.
		 */
		unsigned int mask_left(int bit);

		/**
		 * Calculates the length of a C string.
		 * @param string the string to measure the length of.
		 * @return the length of the string.
		 */
		unsigned int strlen(const char * string);

		/**
		 * Duplicates a C string.
		 * @param source the string to duplicate.
		 * @return a newly allocated string with the same contents as the source string.
		 */
		char * strdup(const char * source);

		/**
		 * Copies a C string.
		 * @param dest the destination buffer.
		 * @param source the source buffer.
		 */
		char * strcpy(char * dest, const char * source);

		/**
		 * Appends a C string onto another.
		 * @param dest the destination string.
		 * @param source the source string.
		 */
		char * strcat(char * dest, const char * source);
	}
}

#endif

