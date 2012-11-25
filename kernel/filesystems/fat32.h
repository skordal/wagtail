// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_FILESYSTEMS_FAT32_H
#define WAGTAIL_FILESYSTEMS_FAT32_H

#include "../device.h"
#include "../direntry.h"
#include "../file.h"
#include "../filesystem.h"
#include "../kstring.h"
#include "../partition.h"

#include "fat32_file.h"

// Uncomment the following define to enable lots and lots of
// debugging messages from the FAT32 driver:
#define WAGTAIL_FS_FAT32_DEBUG

namespace wagtail
{
	namespace filesystems
	{
		// FAT32 filesystem driver.
		class fat32 final : public filesystem
		{
			friend class fat32_file;

			public:
				fat32(partition * part);

				// Gets the amount of free space in the filesystem by reading
				// the FSInfo sector and calculating the free space in bytes:
				unsigned long long get_free_space() const override;

				// Gets the volume ID:
				const kstring & get_label() const override { return volume_label; }

				// Opens a file:
				file * open_file(const kstring & path) override;

				// Reads the specified directory:
				direntry * read_directory(const kstring & path) override;
			private:
				static const unsigned char ATTR_READ_ONLY =	0x01;
				static const unsigned char ATTR_HIDDEN =	0x02;
				static const unsigned char ATTR_SYSTEM = 	0x04;
				static const unsigned char ATTR_VOLUME_ID = 	0x08;
				static const unsigned char ATTR_DIRECTORY = 	0x10;
				static const unsigned char ATTR_ARCHIVE =	0x20;
				static const unsigned char ATTR_LONG_NAME =
					ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID;
				static const unsigned char ATTR_LONG_NAME_MASK =
					ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID|
					ATTR_DIRECTORY|ATTR_ARCHIVE;

				static const unsigned char LAST_LONG_ENTRY =	0x40;

				// Reads the specified directory, starting in the directory specified
				// by the cluster parameter:
				direntry * read_directory(unsigned int cluster);

				// Translates a cluster number into a block address for use with the
				// underlying block device:
				block_device::block_address_t get_cluster_address(unsigned int num) const;

				// "Decodes" and 8.3 filename, which basically means to remove the padding
				// bytes and insert the dot before the extension:
				static kstring decode_83_filename(const kstring & filename);

				// Gets the next cluster number from the FAT for the specified cluster,
				// returns 0 if there is no next cluster, or 1 if an error occurs:
				unsigned int get_next_cluster(unsigned int cluster) const;

				unsigned short bytes_per_sector;
				unsigned short reserved_sectors;
				unsigned char number_of_fats;

				unsigned int sector_count;
				unsigned int sectors_per_fat;
				unsigned int first_root_cluster;
				unsigned char sectors_per_cluster;

				block_device::block_address_t first_data_sector;
				block_device::block_address_t fsinfo_sector;

				unsigned int volume_id;
				kstring volume_label;
		};
	}
}

#endif

