// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_IO_H
#define WAGTAIL_IO_H

namespace wagtail
{
	namespace io
	{
		// Reads one element of the type T from the register located
		// at (base + offset):
		template<typename T>
		inline T read(const void * base, unsigned int offset = 0)
			{ return *(((volatile T *)((unsigned int) base + offset))); }
		
		// Writes one element of the type T to the register located
		// at (base + offset):
		template<typename T>
		inline void write(T data, const void * base, unsigned int offset = 0)
			{ *(((volatile T *) base) + offset) = data; }
	}
}

#endif

