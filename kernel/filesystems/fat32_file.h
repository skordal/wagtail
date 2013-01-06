// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_FILESYSTEMS_FAT32_FILE_H
#define WAGTAIL_FILESYSTEMS_FAT32_FILE_H

#include "../file.h"
#include "../filesystem.h"
#include "../kstring.h"
#include "fat32.h"

namespace wagtail
{
	namespace filesystems
	{
		class fat32;

		/**
		 * Class representing a file on an FAT32 filesystem
		 */
		class fat32_file final : public file
		{
			public:
				/**
				 * Constructs a FAT32 file object.
				 * @param path the path to the file.
				 * @param fs the filesystem where the file lives.
				 * @param size the size of the file.
				 * @param start_cluster the start cluster of the file.
				 * @param read_only whether the file is read-only.
				 */
				fat32_file(const kstring & path, fat32 * fs, unsigned long long size,
					unsigned int start_cluster, bool read_only = true);
				/** Destructs the file object. */
				~fat32_file() {}

				/**
				 * Reads from the file.
				 * @param buffer the target buffer.
				 * @param size the number of bytes to read.
				 * @param offset the offset into the file where the read should begin.
				 * @return `true` if successful, `false` otherwise.
				 */
				bool read(void * buffer, unsigned int size, unsigned int offset = 0) override;
				/**
				 * Writes to the file.
				 * @param buffer the buffer with data to write.
				 * @param size the size of the buffer to write.
				 * @param offset the offset into the at which to write the data.
				 * @return `true` if successful, `false` otherwise.
				 */
				bool write(void * buffer, unsigned int size, unsigned int offset = 0) override;
			private:
				fat32 * fs;
				unsigned int bytes_per_cluster;
		};
	}
}

#endif

