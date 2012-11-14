// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "filesystem.h"
#include "kernel.h"

// Filesystem driver headers:
#include "filesystems/fat32.h"

using namespace wagtail;
using namespace wagtail::filesystems;

filesystem * filesystem::initialize(partition * part)
{
	filesystem * fs = nullptr;
	kernel::message() << part->get_name() << ": filesystem type: " << (void *) (int) part->get_type() << kstream::newline;

	switch(part->get_type())
	{
		case 0xc:
			kernel::message() << part->get_name() << ": trying FAT driver" << kstream::newline;
			fs = new filesystems::fat32(part);
			break;
		default:
			kernel::message() << "Unrecognized filesystem type: "
				<< (void *) (int) part->get_type() << kstream::newline;
	}

	if(fs != nullptr && !fs->is_usable())
	{
		delete fs;
		return nullptr;
	} else
		return fs;
}

