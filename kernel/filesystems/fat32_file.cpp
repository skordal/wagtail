// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "../kernel.h"
#include "fat32_file.h"

using namespace wagtail;
using namespace wagtail::filesystems;

fat32_file::fat32_file(const kstring & path, fat32 * fs, unsigned long long size,  unsigned int start_cluster, bool read_only)
	: file(path, fs, size, start_cluster, read_only), fs(fs),
	bytes_per_cluster(fs->bytes_per_sector << (fs->sectors_per_cluster - 1))
{
	kernel::message() << "Bytes per cluster: " << (int) bytes_per_cluster << kstream::newline;
}

bool fat32_file::read(void * buffer, unsigned int size, unsigned int offset)
{
	int cluster = get_start_inode();
	unsigned int bytes_read = 0;
	bool retval = false;

	for(; offset > bytes_per_cluster; offset -= bytes_per_cluster)
	{
		cluster = fs->get_next_cluster(cluster);

		if(cluster == 0 || cluster == 1)
		{
			kernel::message() << get_path() << ": cannot read " << (int) size
				<< " bytes at offset " << (int) offset << ": end of file reached."
				<< kstream::newline;
			return false;
		}
	}

	char * temp_buffer = new char[bytes_per_cluster];
	while(bytes_read < size)
	{
		fs->get_partition()->read_blocks(temp_buffer, fs->get_cluster_address(cluster), fs->sectors_per_cluster);
		for(unsigned int j = offset; j < bytes_per_cluster && bytes_read < size; ++j, ++bytes_read)
			*((char *) buffer + bytes_read) = temp_buffer[j];
		offset = 0;

		cluster = fs->get_next_cluster(cluster);
		if((cluster == 0 || cluster == 1) && bytes_read < size)
		{
			kernel::message() << get_path() << ": read error: end of file reached."
				<< kstream::newline;
			break;
		}
	}
	delete[] temp_buffer;

	return retval;
}

bool fat32_file::write(void * buffer, unsigned int size, unsigned int offset)
{
	if(is_read_only())
	{
		kernel::message() << get_path() << ": cannot write to this file, it is read-only!"
			<< kstream::newline;
		return false;
	}

	// TODO: Implement FAT32 write support.
	return false;
}

