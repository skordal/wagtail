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

		class fat32_file final : public file
		{
			public:
				fat32_file(const kstring & path, fat32 * fs, unsigned long long size,
					unsigned int start_cluster, bool read_only = true);
				~fat32_file() {}

				bool read(void * buffer, unsigned int size, unsigned int offset = 0) override;
				bool write(void * buffer, unsigned int size, unsigned int offset = 0) override;
			private:
				fat32 * fs;
		};
	}
}

#endif

