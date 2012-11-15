// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTRING_H
#define WAGTAIL_KSTRING_H

#include "utils.h"

namespace wagtail
{
	// A simple string class for simplifying working with strings. Loosely based on
	// std::string.
	class kstring
	{
		public:
			kstring(const char * string);
			kstring(const kstring & string);
			kstring();

			~kstring();

			// Clears the string, i.e. sets its value to "":
			void clear() { delete[] buffer; buffer = new char[1]; buffer[0] = 0; }

			// Appends a string to the end of this string:
			void append(const char * string);
			void append(const kstring & string);

			// Prepends a string to the beginning of this string:
			void prepend(const char * string);
			void prepend(const kstring & string);

			// String concatenation operators:
			kstring & operator+=(const char * string);
			kstring & operator+=(const kstring & string);

			// Index operator:
			char & operator[](int n) { return buffer[n]; }
			char operator[](int n) const { return buffer[n]; }

			unsigned int get_length() const { return utils::strlen(buffer); }
			bool is_empty() const { return get_length() == 0; }
			const char * get_buffer() const { return buffer; }
		private:
			char * buffer;
	};
}

#endif

