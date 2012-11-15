// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kstring.h"

using namespace wagtail;

kstring::kstring(const char * string)
{
	buffer = utils::strdup(string);
}

kstring::kstring(const kstring & string)
{
	buffer = utils::strdup(string.buffer);
}

kstring::kstring()
{
	buffer = new char[1];
	buffer[0] = 0;
}

kstring::~kstring()
{
	delete[] buffer;
}

void kstring::append(const char * string)
{
	// Do not append empty strings, just return:
	if(utils::strlen(string) == 0)
		return;

	char * new_buffer = new char[utils::strlen(buffer) + utils::strlen(string) + 1];

	utils::strcpy(new_buffer, buffer);
	utils::strcat(new_buffer, string);

	delete[] buffer;
	buffer = new_buffer;
}

void kstring::append(const kstring & string)
{
	append(string.buffer);
}

void kstring::prepend(const char * string)
{
	if(utils::strlen(string) == 0)
		return;

	char * new_buffer = new char[utils::strlen(buffer) + utils::strlen(string) + 1];

	utils::strcpy(new_buffer, string);
	utils::strcat(new_buffer, buffer);

	delete[] buffer;
	buffer = new_buffer;
}

void kstring::prepend(const kstring & string)
{
	prepend(string.buffer);
}

kstring & kstring::operator+=(const char * string)
{
	append(string);
	return *this;
}

kstring & kstring::operator+=(const kstring & string)
{
	append(string);
	return *this;
}

