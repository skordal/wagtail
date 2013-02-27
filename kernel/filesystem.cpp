// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "filesystem.h"
#include "kernel.h"

// Filesystem driver headers:
#include "filesystems/fat32.h"

using namespace wagtail;
using namespace wagtail::filesystems;

// Initializes a filesystem for the specified partition.
filesystem * filesystem::initialize(partition * part, char type)
{
	filesystem * fs = nullptr;

	switch(type != partition::auto_type ? type : part->get_type())
	{
		case 0xc:
			kernel::message() << part->get_name() << ": trying FAT32 driver" << kstream::newline;
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

// Checks if a specified file/directory exists in the filesystem:
direntry * filesystem::file_exists(const kstring & path)
{
	direntry * dir = read_directory(path.dirname());
	if(dir == nullptr)
		return nullptr;

	direntry * retval = direntry::find_entry(path.filename(), dir);
	if(retval == nullptr)
	{
		direntry::free_list(dir);
		return nullptr;
	}

	// Copy the found file:
	retval = new direntry(*retval);
	direntry::free_list(dir);
	return retval;
}

