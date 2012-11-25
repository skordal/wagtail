// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "fat32.h"
#include "../kernel.h"

using namespace wagtail;
using namespace wagtail::filesystems;

fat32::fat32(partition * part) : filesystem(part)
{
	char label_buffer[12] = {0};
	char * buffer = new char[part->get_block_size()];

	// Read the volume header sector:
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
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": bytes per sector: " << (int) bytes_per_sector << kstream::newline;
#endif

	// Get the number of sectors per cluster:
	sectors_per_cluster = buffer[13];
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": sectors per cluster: " << (int) sectors_per_cluster << kstream::newline;
#endif

	// Get the number of reserved sectors:
	reserved_sectors = buffer[14] | buffer[15] << 8;
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": reserved sectors: " << (int) reserved_sectors << kstream::newline;
#endif

	// Get the number of FATs:
	number_of_fats = buffer[16];
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": number of FATs: " << (int) number_of_fats << kstream::newline;
#endif

	// Get the sector count:
	sector_count = *((int *) (buffer + 32));
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": sector count: " << (void *) sector_count << kstream::newline;
#endif
	if(sector_count != part->get_size())
	{
		kernel::message() << part->get_name() << ": sector count does not match the partition size!"
			<< kstream::newline;
		set_usable(false);
		goto _finish_init;
	}

	// Get the size of a FAT:
	sectors_per_fat = *((int *) (buffer + 36));
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": sectors per FAT: " << (void *) sectors_per_fat << kstream::newline;
#endif

	// Get the first root directory cluster:
	first_root_cluster = *((int *) (buffer + 44));
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": first root cluster: " << (void *) first_root_cluster << kstream::newline;
#endif

	// Set the sector address of the FSInfo sector:
	fsinfo_sector = (buffer[48] | buffer[49] << 8) + part->get_start_address();
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": FSInfo sector: " << (void *) fsinfo_sector << kstream::newline;
#endif

	// Get the volume ID:
	volume_id = buffer[67]|buffer[68] << 8|buffer[69] << 16|buffer[70] << 24;
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": volume ID: " << (void *) volume_id << kstream::newline;
#endif

	// Get the volume label:
	for(int i = 0; i < 11; ++i)
		label_buffer[i] = buffer[71 + i];
	volume_label = label_buffer;
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": volume label: " << volume_label << kstream::newline;
#endif

	// Set the sector address of the first data sector:
	first_data_sector = reserved_sectors + (number_of_fats * sectors_per_fat) + part->get_start_address();
#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << part->get_name() << ": first data sector: " << (void *) first_data_sector << kstream::newline;
#endif

	// Initialization succeeded, mark the filesystem as usable before continuing:
	set_usable(true);

_finish_init:
	delete[] buffer;
}

unsigned long long fat32::get_free_space() const
{
	unsigned long long retval = 0;
	char * buffer = new char[get_partition()->get_block_size()];
	get_partition()->read_block(buffer, fsinfo_sector);

#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << get_partition()->get_name() << ": FSInfo lead signature: "
		<< (void *) *((int *) buffer) << kstream::newline;
	kernel::message() << get_partition()->get_name() << ": FSSInfo structure signature: "
		<< (void *) *((int *) (buffer + 484)) << kstream::newline;
#endif

	// Get the count of free clusters:
	unsigned int free_count = *((int *) (buffer + 488));
	if(free_count == 0xffffffff)
		kernel::message() << get_partition()->get_name() << ": number of free clusters is unknown!"
			<< kstream::newline;
#ifdef WAGTAIL_FS_FAT32_DEBUG
	else
		kernel::message() << get_partition()->get_name() << ": number of free clusters: "
			<< (void *) free_count << kstream::newline;
#endif

	// Convert the free block count into bytes:
	if(free_count != 0xffffffff) // 0xffffffff = unknown, should be computed
		retval = sectors_per_cluster * free_count * get_partition()->get_block_size();

	delete[] buffer;
	return retval;
}

file * fat32::open_file(const kstring & path)
{
	kstring dir = path.dirname();
	kstring filename = path.filename();

	direntry * dir_contents = read_directory(dir);
	if(dir_contents == nullptr)
	{
		kernel::message() << "File not found: " << path << ": directory does not exist!"
			<< kstream::newline;
		return nullptr;
	}

	direntry * file = direntry::find_entry(filename, dir_contents);
	if(file == nullptr)
	{
		kernel::message() << "File not found: " << path << ": file does not exist!"
			<< kstream::newline;
		direntry::free_list(dir_contents);
		return nullptr;
	}

	wagtail::file * retval = new fat32_file(path, this, file->get_size(), file->get_inode());
	direntry::free_list(dir_contents);

	return retval;
}

direntry * fat32::read_directory(const kstring & path)
{
	kstring mutable_path(path[0] == '/' ? path.get_buffer() + 1 : path);
	direntry * retval = nullptr;

	// If / is requested, return it directly:
	if(path == "/" || mutable_path == "")
		return read_directory(2);

	direntry * current = read_directory(2);
	while(current != nullptr)
	{
		unsigned long directory_cluster;
		int slash_index = mutable_path.index('/');

		// If we are at the last path component, check if the desired directory
		// is available; if it is, read it and return the result:
		if(slash_index == kstring::npos)
		{
			direntry * temp = direntry::find_entry(mutable_path, current);
			if(temp != nullptr)
			{
				retval = read_directory(temp->get_inode());
				direntry::free_list(current);
			}

			break;
		}

		kstring dirname = kstring(mutable_path, 0, slash_index);

		// Find the next entry in the path:
		direntry * next_entry = direntry::find_entry(dirname, current);
		if(next_entry == nullptr || next_entry->get_type() != direntry::type::directory)
			break;

		// Read the next directory:
		directory_cluster = next_entry->get_inode();
		direntry::free_list(current);
		current = read_directory(directory_cluster);

		mutable_path = mutable_path.get_buffer() + slash_index + 1;
	}

	return retval;
}

// Reads the directory located at the specific cluster:
direntry * fat32::read_directory(unsigned int cluster)
{
	direntry * retval = nullptr;
	direntry * previous;
	kstring long_filename;
	bool finished = false;

#ifdef WAGTAIL_FS_FAT32_DEBUG
	kernel::message() << get_partition()->get_name() << ": reading directory starting in cluster "
		<< (void *) cluster << kstream::newline;
#endif

	// Read the directory listing:
	do {
		char * buffer = new char[get_partition()->get_block_size() * sectors_per_cluster];
		get_partition()->read_blocks(buffer, get_cluster_address(cluster), sectors_per_cluster);

		for(unsigned int i = 0; i < get_partition()->get_block_size(); i += 32)
		{
			char entry_name[13] = {0};
			unsigned int file_cluster =
				buffer[i + 20] << 16|buffer[i + 21] << 24| // High word
				buffer[i + 26]|buffer[i + 27] << 8;        // Low word
			unsigned int size = *((int *) (buffer + i + 28));
			direntry::type entry_type = direntry::type::file;

			if(buffer[i] == 0xe5)		// Ignore empty entries
			{
				if(long_filename.get_length() != 0)
					long_filename.clear();
				continue;
			} else if(buffer[i] == 0x00)	// Stop when reaching end-of-directory
			{
				finished = true;
				break;
			}

			// Accumulate long filenames and use the instead of the 8.3 filenames.
			// FIXME: This code currently ignores unicode, checksums and entry sequence numbers.
			if((buffer[i + 11] & ATTR_LONG_NAME) == ATTR_LONG_NAME)
			{
				char temp[14] = {0};
				int temp_index = 0;

				// Get the long filename contents, ignoring the 8 extra bits used
				// for storing unicode information:
				for(int j = 1; j < 11; j += 2, ++temp_index)
					temp[temp_index] += buffer[i + j];
				for(int j = 14; j < 26; j += 2, ++temp_index)
					temp[temp_index] += buffer[i + j];
				for(int j = 28; j < 32; j += 2, ++temp_index)
					temp[temp_index] += buffer[i + j];
				long_filename.prepend(temp);
				continue;
			}

			// Read the 8.3 filename:
			for(int j = 0; j < 11; ++j)
				entry_name[j] = buffer[i + j];

			// Check if the entry is a directory:
			if(buffer[i + 11] & ATTR_DIRECTORY)
				entry_type = direntry::type::directory;

			// Insert the directory into the return list:
			if(retval == nullptr)
			{
				if(long_filename.get_length() != 0)
					retval = new direntry(long_filename, entry_type, this, file_cluster, size, 00444);
				else
					retval = new direntry(decode_83_filename(entry_name),
						entry_type, this, file_cluster, size, 00444);
				long_filename.clear();
				previous = retval;
			} else {
				direntry * new_entry;
				if(long_filename.get_length() != 0)
					new_entry = new direntry(long_filename, entry_type, this, file_cluster, size, 00444);
				else
					new_entry = new direntry(decode_83_filename(entry_name), entry_type, this,
						file_cluster, size, 00444);
				long_filename.clear();

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
			if(cluster == 0)
				finished = true;
			else if(cluster == 1) // If an error occurs, something is wrong with the filesystem.
			{
				// Free the previously allocated entries, if any:
				direntry * current = retval;
				while(current != nullptr)
				{
					direntry * next = current->get_next();
					delete current;
					current = next;
				}

				finished = true;
				return nullptr;
			}
		}
	} while(!finished);

	return retval;
}

block_device::block_address_t fat32::get_cluster_address(unsigned int cluster) const
{
	return ((cluster - 2) * sectors_per_cluster) + first_data_sector;
}

kstring fat32::decode_83_filename(const kstring & filename)
{
	kstring retval;
	for(unsigned int i = 0; i < filename.get_length(); ++i)
	{
		// Strip out the padding bits:
		if(filename[i] != 0x20)
			retval += filename[i];
	}

	// TODO: Add the dot. Also note that this method has not been tested yet, since everyone
	// TODO: uses long filenames nowadays.

	return retval;
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

