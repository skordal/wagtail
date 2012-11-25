// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "vfs.h"

using namespace wagtail;

vfs * vfs::vfs_interface = nullptr;

void vfs::initialize()
{
	kernel::message() << "Initializing VFS..." << kstream::newline;
	vfs_interface = new vfs;
}

vfs::vfs() : filesystem(nullptr), vfs_label("Virtual Filesystem")
{

}

filesystem * vfs::mount(const kstring & path, partition * part, char partition_type)
{
	filesystem * retval = filesystem::initialize(part, partition_type);
	if(retval == nullptr)
	{
		kernel::message() << "VFS: cannot mount filesystem on " << path
			<< ": the filesystem could not be used" << kstream::newline;
		return nullptr;
	}

	if(mount_list == nullptr && path != "/")
	{
		kernel::message() << "VFS: cannot mount filesystem on " << path
			<< " before / has been mounted" << kstream::newline;
		delete retval;
		return nullptr;
	}

	if(path == "/")
		set_usable();

	mountpoint * m = new mountpoint(path, retval);
	add_mountpoint(m);

	return retval;
}

void vfs::unmount(const kstring & path)
{
	if(path == "/")
		set_usable(false);

	// TODO: Write the rest.
}

unsigned long long vfs::get_free_space() const
{
	unsigned long long retval = 0;
	mountpoint * current = mount_list;
	while(current != nullptr)
	{
		retval += current->get_filesystem()->get_free_space();
		current = current->get_next();
	}

	return retval;
}

filesystem * vfs::get_filesystem(const kstring & path) const
{
	return nullptr;
}

file * vfs::open_file(const kstring & path)
{
	mountpoint * current = mount_list;

	while(current != nullptr)
	{
		kstring prefix = kstring(path, 0, current->get_path().get_length());
		if(prefix == current->get_path())
		{
			if(prefix == "/")
				return current->get_filesystem()->open_file(path);
			else
				return current->get_filesystem()->open_file(
					kstring(path, current->get_path().get_length(), path.get_length()));
		}

		current = current->get_next();
	}

	return nullptr;
}

direntry * vfs::read_directory(const kstring & path)
{
	mountpoint * current = mount_list;

	while(current != nullptr)
	{
		kstring prefix = kstring(path, 0, current->get_path().get_length());
		if(prefix == current->get_path())
		{
			if(prefix == "/")
				return current->get_filesystem()->read_directory(path);
			else
				return current->get_filesystem()->read_directory(
					kstring(path, current->get_path().get_length(), path.get_length()));
		}

		current = current->get_next();
	}

	return nullptr;
}

void vfs::print_mountlist()
{
	mountpoint * current = mount_list;
	while(current != nullptr)
	{
		kernel::message() << "Mountpoint: " << current->get_path() << " - partition: "
			<< current->get_filesystem()->get_partition()->get_name() << kstream::newline;
		current = current->get_next();
	}
}

void vfs::add_mountpoint(mountpoint * m)
{
	// TODO: Check if the mountpoint/partition is in use.

	if(mount_list == nullptr)
		mount_list = m;
	else {
		mountpoint * current = mount_list;
		while(current->get_next() != nullptr)
		{
			if(current->get_path().get_length() < m->get_path().get_length())
				break;
			current = current->get_next();
		}

		if(current == mount_list)
			mount_list = m;

		m->set_next(current);
		m->set_prev(current->get_prev());
		if(current->get_prev() != nullptr)
			current->get_prev()->set_next(m);
		current->set_prev(m);
	}

}


