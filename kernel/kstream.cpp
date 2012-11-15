// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kstream.h"

using namespace wagtail;

// Newline character:
const char * kstream::newline = "\n\r";

// Puts a string into a stream:
kostream & kostream::operator << (const char * string)
{
	for(int i = 0; string[i] != 0; ++i)
		*this << string[i];
	return *this;
}

// Puts a kstring into a stream:
kostream & kostream::operator << (const kstring & string)
{
	*this << string.get_buffer();
	return *this;
}

// Puts a hexadecimal address into a stream:
kostream & kostream::operator << (void * pointer)
{
	static const char * hex_digits = "0123456789abcdef";
	*this << '0' << 'x';

	for(int i = 28; i >= 0; i -= 4)
		*this << hex_digits[reinterpret_cast<unsigned int >(pointer) >> i & 0xf];

	return *this;
}

// Prints a signed int to the stream:
kostream & kostream::operator << (int number)
{
	static char buffer[10];
	bool inside_num = false;

	if(number & 0x80000000)
	{
		number = ~number;
		++number;
		*this << '-';
	}

	for(int i = 0; i < 10; ++i)
		buffer[i] = '0';

	for(int i = 9; i >= 0; --i)
	{
		buffer[i] = '0' + (number % 10);
		number /= 10;
	}

	for(int i = 0; i < 10; ++i)
	{
		// Do not print leading zeroes:
		if(!inside_num && buffer[i] == '0' && i != 9)
			continue;
		inside_num = true;
		*this << buffer[i];
	}

	return *this;
}

// Reads a string for a stream:
kistream & kistream::operator >> (const char * string)
{
	// TODO: Implement when proper memory management has been implemented.
	return *this;
}

