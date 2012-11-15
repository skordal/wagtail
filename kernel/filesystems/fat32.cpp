// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "fat32.h"
#include "../kernel.h"

using namespace wagtail;
using namespace wagtail::filesystems;

const char fat32::reserved_characters[16] =
	{
		0x22, 0x2a, 0x2b, 0x2c, 0x2e, 0x2f, 0x3a, 0x3b,
		0x3c, 0x3d, 0x3e, 0x3f, 0x5b, 0x5c, 0x5d, 0x7c
	};

fat32::fat32(partition * part) : filesystem(part)
{
	char label_buffer[12] = {0};
	char * buffer = new char[part->get_block_size()];
	part->read_block(buffer, part->get_start_address());

	// Check for 0xeb or 0xe9 in the first byte:
	if(buffer[0] != 0xeb && buffer[0] != 0xe9)
	{
		kernel::message() << part->get_name() << ": does not seem to be a FAT volume!" << kstream::newline;
		set_usable(false);
		goto _finish_init;
	}

	// Get the number of bytes per sector:
	bytes_per_sector = buffer[11] | buffer[12] << 8;
	if(bytes_per_sector != part->get_block_size())
	{
		kernel::message() << part->get_name() << ": using " << (int) bytes_per_sector
			<< " bytes per sector is not supported!" << kstream::newline
			<< part->get_name() << ": bytes per sector must match the block device's block size."
			<< kstream::newline;
		set_usable(false);
		goto _finish_init;
	}
	kernel::message() << part->get_name() << ": bytes per sector: " << (int) bytes_per_sector << kstream::newline;

	// Get the number of sectors per cluster:
	sectors_per_cluster = buffer[13];
	kernel::message() << part->get_name() << ": sectors per cluster: " << (int) sectors_per_cluster << kstream::newline;

	// Get the number of reserved sectors:
	reserved_sectors = buffer[14] | buffer[15] << 8;
	kernel::message() << part->get_name() << ": reserved sectors: " << (int) reserved_sectors << kstream::newline;

	// Get the number of FATs:
	number_of_fats = buffer[16];
	kernel::message() << part->get_name() << ": number of FATs: " << (int) number_of_fats << kstream::newline;

	// Get the sector count:
	sector_count = *((int *) (buffer + 32));
	kernel::message() << part->get_name() << ": sector count: " << (void *) sector_count << kstream::newline;
	if(sector_count != part->get_size())
	{
		kernel::message() << part->get_name() << ": sector count does not match the partition size!"
			<< kstream::newline;
		set_usable(false);
		goto _finish_init;
	}

	// Get the size of a FAT:
	sectors_per_fat = *((int *) (buffer + 36));
	kernel::message() << part->get_name() << ": sectors per FAT: " << (void *) sectors_per_fat << kstream::newline;

	// Get the first root directory cluster:
	first_root_cluster = *((int *) (buffer + 44));
	kernel::message() << part->get_name() << ": first root cluster: " << (void *) first_root_cluster << kstream::newline;

	// Set the sector address of the FSInfo sector:
	fsinfo_sector = (buffer[48] | buffer[49] << 8) + part->get_start_address();
	kernel::message() << part->get_name() << ": FSInfo sector: " << (void *) fsinfo_sector << kstream::newline;

	// Get the volume ID:
	volume_id = buffer[67]|buffer[68] << 8|buffer[69] << 16|buffer[70] << 24;
	kernel::message() << part->get_name() << ": volume ID: " << (void *) volume_id << kstream::newline;

	// Get the volume label:
	for(int i = 0; i < 11; ++i)
		label_buffer[i] = buffer[71 + i];
	volume_label = label_buffer;
	kernel::message() << part->get_name() << ": volume label: " << volume_label << kstream::newline;

	// Set the sector address of the first data sector:
	first_data_sector = reserved_sectors + (number_of_fats * sectors_per_fat) + part->get_start_address();
	kernel::message() << part->get_name() << ": first data sector: " << (void *) first_data_sector << kstream::newline;

	set_usable(true);

_finish_init:
	delete[] buffer;
}

unsigned long long fat32::get_free_space() const
{
	unsigned long long retval = 0;
	char * buffer = new char[get_partition()->get_block_size()];
	get_partition()->read_block(buffer, fsinfo_sector);

	kernel::message() << get_partition()->get_name() << ": FSInfo lead signature: "
		<< (void *) *((int *) buffer) << kstream::newline;
	kernel::message() << get_partition()->get_name() << ": FSSInfo structure signature: "
		<< (void *) *((int *) (buffer + 484)) << kstream::newline;

	// Get the count of free clusters:
	unsigned int free_count = *((int *) (buffer + 488));
	if(free_count == 0xffffffff)
		kernel::message() << get_partition()->get_name() << ": number of free clusters is unknown!"
			<< kstream::newline;
	else
		kernel::message() << get_partition()->get_name() << ": number of free clusters: "
			<< (void *) free_count << kstream::newline;

	// Compute the amount of free space in bytes:
	if(free_count != 0xffffffff) // 0xffffffff = unknown, should be computed
		retval = sectors_per_cluster * free_count * get_partition()->get_block_size();

	delete[] buffer;
	return retval;
}

direntry * fat32::read_directory(const kstring & path)
{
	if(path[0] != '/')
	{
		kernel::message() << get_partition()->get_name() << ": path name should start with '/': "
			<< path << kstream::newline;
		return nullptr;
	}

	// TODO: This function only reads the root directory for now
	return read_directory(2);
}

direntry * fat32::read_directory(unsigned int cluster)
{
	direntry * retval = nullptr;
	direntry * previous;
	bool finished = false;

	// Read the directory listing:
	do {
		char * buffer = new char[get_partition()->get_block_size()];
		get_partition()->read_block(buffer, get_cluster_address(cluster));

		for(unsigned int i = 0; i < get_partition()->get_block_size(); i += 32)
		{
			char entry_name[13] = {0};
			unsigned int size = *((int *) (buffer + i + 28));
			direntry::type entry_type = direntry::type::file;

			if(buffer[i] == 0xe5)		// Ignore empty entries
			{
				continue;
			} else if(buffer[i] == 0x00)	// Stop when reaching end-of-directory
			{
				finished = true;
				break;
			}

			// Skip long name entries:
			if((buffer[i + 11] & ATTR_LONG_NAME) == ATTR_LONG_NAME)
				continue;

			// Read the directory name (8.3 filename only for now);
			// TODO: add a dot before the extension and strip out padding 0x20 bytes.
			for(int j = 0; j < 11; ++j)
				entry_name[j] = buffer[i + j];

			if(buffer[i + 11] & ATTR_DIRECTORY)
				entry_type = direntry::type::directory;

			// Insert the directory into the return list:
			if(retval == nullptr)
			{
				retval = new direntry(entry_name, entry_type, this, size, 00444);
				previous = retval;
			} else {
				direntry * new_entry = new direntry(entry_name, entry_type, this, size, 00444);
				previous->set_next(new_entry);
				previous = new_entry;
			}
		}
		delete[] buffer;

		// If the directory listing continues into another cluster, look up the next cluster
		// number:
		if(!finished)
		{
			cluster = get_next_cluster(cluster);

			// If there are no more clusters, exit the loop:
			if(cluster == 0 || cluster == 1)
				finished = true;
		}
	} while(!finished);

	return retval;
}

block_device::block_address_t fat32::get_cluster_address(unsigned int cluster) const
{
	return ((cluster - 2) * sectors_per_cluster) + first_data_sector;
}

unsigned int fat32::get_next_cluster(unsigned int cluster) const
{
	block_device::block_address_t fat_address = get_partition()->get_start_address() + reserved_sectors;
	unsigned int fat_offset = cluster << 2;
	unsigned int retval;

	// Read the correct sector of the FAT:
	fat_address += (fat_offset >> 9);

	// Correct the FAT offset for when the entry is not in FAT sector 0:
	fat_offset &= 0x1ff;

	char * buffer = new char[get_partition()->get_block_size()];
	if(!get_partition()->read_block(buffer, fat_address))
	{
		delete[] buffer;
		return 1;
	}

	retval = *((int *) (buffer + fat_offset));
	delete[] buffer;

	retval &= 0x0fffffff;

	if(retval >= 0x0ffffff8)
		return 0;
	else
		return retval;
}

