// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "direntry.h"

using namespace wagtail;

direntry::direntry(const kstring & name, type entry_type, filesystem * fs, unsigned long long file_size,
	unsigned short permissions, unsigned short owner, unsigned short group)
	: name(name), entry_type(entry_type), fs(fs), file_size(file_size),
	permissions(permissions), owner(owner), group(group)
{
}

