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

		// Reads an element of the specified type T from the register located at
		// the specified base + offset, does a bitwise OR of its value and the
		// provided value, and writes it back to the register:
		template<typename T>
		inline void or_register(T data, const void * base, unsigned int offset = 0)
			{ T temp = read<T>(base, offset); temp |= data; write(temp, base, offset); }
	}
}

#endif

