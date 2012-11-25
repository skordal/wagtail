// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "file.h"

using namespace wagtail;

file::file(const kstring & path, filesystem * fs, unsigned long long size, unsigned int start_inode, bool read_only)
	: path(path.dirname()), fs(fs), size(size), start_inode(start_inode), read_only(read_only)
{

}

