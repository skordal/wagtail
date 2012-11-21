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

			// Finds the direntry containing a specified filename by starting at the
			// specified direntry and searching through the linked list. Returns
			static direntry * find_entry(const kstring & name, direntry * start);

			// Frees a linked direntry list:
			static void free_list(direntry * start);

			direntry(const kstring & name, direntry::type file_type, filesystem * fs,
				unsigned int inode, unsigned long long file_size = 0,
				unsigned short permissions = 00666, unsigned short owner = 0,
				unsigned short group = 0);
			direntry(const direntry & entry);

			void set_next(direntry * next) { this->next = next; }
			direntry * get_next() const { return next; }

			const kstring & get_name() const { return name; }
			direntry::type get_type() const { return entry_type; }
			unsigned long long get_size() const { return file_size; }
			unsigned int get_inode() const { return inode; }
		private:
			kstring name;
			type entry_type;

			direntry * next = nullptr;
			filesystem * fs;

			unsigned int inode;
			unsigned long long file_size;
			unsigned short permissions;
			unsigned short owner, group;
	};
}

#endif

