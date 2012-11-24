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

kstring::kstring(const kstring & string, int start_offset, int end_offset)
{
	int length = end_offset - start_offset;

	if(start_offset + end_offset > (int) string.get_length())
		length = string.get_length() - start_offset;

	buffer = new char[length + 1];
	for(int i = 0; i < length; ++i)
		buffer[i] = string[start_offset + i];
	buffer[length] = 0;
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

void kstring::tolower()
{
	for(unsigned int i = 0; i < get_length(); ++i)
	{
		if(buffer[i] >= 'A' && buffer[i] <= 'Z')
			buffer[i] = (buffer[i] - 'A') + 'a';
	}
}

void kstring::toupper()
{
	for(unsigned int i = 0; i < get_length(); ++i)
	{
		if(buffer[i] >= 'a' && buffer[i] <= 'z')
			buffer[i] = (buffer[i] - 'a') + 'A';
	}
}

kstring kstring::dirname() const
{
	int last_slash = rindex('/');
	if(last_slash == kstring::npos)
		return "";
	else
		return kstring(*this, 0, last_slash);
}

kstring kstring::filename() const
{
	int last_slash = rindex('/');
	if(last_slash == kstring::npos)
		return "";
	else
		return kstring(buffer + last_slash + 1);
}

int kstring::index(char c) const
{
	for(unsigned int i = 0; i < get_length(); ++i)
		if(buffer[i] == c)
			return i;

	return kstring::npos;
}

int kstring::rindex(char c) const
{
	for(unsigned int i = get_length(); i >= 0; --i)
		if(buffer[i] == c)
			return i;

	return kstring::npos;
}

kstring & kstring::operator=(const char * string)
{
	char * old_buffer = buffer;
	buffer = utils::strdup(string);
	delete[] old_buffer; // Don't delete the buffer until after the assignment,
	                     // in case we are assigning from ourselves.
	return *this;
}

kstring & kstring::operator=(const kstring & string)
{
	return (*this = string.buffer);
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

kstring & kstring::operator+=(char c)
{
	char buffer[2] = {c, 0};
	append(buffer);
	return *this;
}

bool kstring::operator==(const kstring & string) const
{
	if(string.get_length() != get_length())
		return false;
	else {
		for(unsigned int i = 0; i < get_length(); ++i)
			if(string[i] != buffer[i])
				return false;
		return true;
	}
}

