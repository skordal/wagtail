// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_DIRENTRY_H
#define WAGTAIL_DIRENTRY_H

#include "filesystem.h"
#include "kstring.h"
#include "utils.h"

namespace wagtail
{
	class filesystem;

	// Class representing a directory entry.
	class direntry
	{
		public:
			enum class type
			{
				file, directory, character_device, block_device, symbolic_link
			};

			direntry(const kstring & name, direntry::type file_type, filesystem * fs,
				unsigned long long file_size = 0, unsigned short permissions = 00666,
				unsigned short owner = 0, unsigned short group = 0);

			void set_next(direntry * next) { this->next = next; }
			direntry * get_next() const { return next; }

			const kstring & get_name() const { return name; }
			direntry::type get_type() const { return entry_type; }
			unsigned long long get_size() const { return file_size; }
		private:
			kstring name;
			type entry_type;

			direntry * next = nullptr;
			filesystem * fs;

			unsigned long long file_size;
			unsigned short permissions;
			unsigned short owner, group;
	};
}

#endif

