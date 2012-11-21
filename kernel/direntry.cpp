// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "direntry.h"
#include "kernel.h"

using namespace wagtail;

// Finds a directory entry in a linked directory entry list:
direntry * direntry::find_entry(const kstring & name, direntry * start)
{
	// Ignore empty lists:
	if(start == nullptr)
		return nullptr;

	// Search the list for direntries matching the specified name:
	for(auto current = start; current != nullptr; current = current->get_next())
		if(current->get_name() == name)
			return current;

	return nullptr;
}

// Deletes all direntries in the specified linked list:
void direntry::free_list(direntry * start)
{
	direntry * current = start;
	while(current != nullptr)
	{
		direntry * temp = current;
		current = current->get_next();
		delete temp;
	}
}

direntry::direntry(const kstring & name, type entry_type, filesystem * fs, unsigned int inode,
	unsigned long long file_size, unsigned short permissions, unsigned short owner, unsigned short group)
	: name(name), entry_type(entry_type), fs(fs), inode(inode), file_size(file_size),
	permissions(permissions), owner(owner), group(group)
{
}

direntry::direntry(const direntry & entry)
{
	name = entry.name;
	entry_type = entry.entry_type;
	fs = entry.fs;
	inode = entry.inode;
	file_size = entry.file_size;
	permissions = entry.permissions;
	owner = entry.owner;
	group = entry.group;
}

