// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTREAM_H
#define WAGTAIL_KSTREAM_H

#include "kstring.h"

namespace wagtail
{
	// Kernel output stream:
	class kostream
	{
		public:
			// Puts a character into the stream; to use the stream class, there
			// is no need to override any other operator than this:
			virtual kostream & operator << (char character) = 0;
			// Puts a string into the stream:
			virtual kostream & operator << (const char * string);
			// Puts a kstring into the stream:
			virtual kostream & operator << (const kstring & string);
			// Prints the hexadecimal address of a pointer:
			virtual kostream & operator << (void * pointer);
			// Prints a decimal number:
			virtual kostream & operator << (int value);
	};

	// Kernel input stream:
	class kistream
	{
		public:
			// Reads a character from the stream; the other operators use this
			// function, so to use this class, only this operator needs to be overridden:
			virtual kistream & operator >> (char & character) = 0;
			// Reads an entire string from the stream, up until the character set previously
			// as deliminator is encountered:
			virtual kistream & operator >> (const char * string);

			// Sets the deliminator
			void set_deliminator(char c) { delim = c; }
			char get_deliminator() const { return delim; }
		protected:
			kistream(char delim = '\n') : delim(delim) {}
		private:
			char delim;
	};

	// Constants for use with streams:
	namespace kstream
	{
		extern const char * newline;
	}
}

#endif

