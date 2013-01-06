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

	/**
	 * Class representing an entry in a directory.
	 * 
	 * Directory entries also contains a pointer to the next entry in a directory, which is used
	 * to return linked lists of directory entries from functions that list the contents of
	 * directories.
	 */
	class direntry
	{
		public:
			/** Type of directory entry. */
			enum class type
			{
				/** A normal file. */
				file,
				/** A directory. */
				directory,
				/** A character device file. */
				character_device,
				/** A block device file. */
				block_device,
				/** A symbolic link to another file. */
				symbolic_link
			};

			/**
			 * Finds the specified entry in a list of directory entries.
			 * @param name the name of the entry to search for.
			 * @param start the start of the directory listing.
			 * @return if found, the entry with the specified name, `nullptr` otherwise.
			 */
			static direntry * find_entry(const kstring & name, direntry * start);

			/**
			 * Frees a linked list of directory entries.
			 * @param start the beginning of the list.
			 */
			static void free_list(direntry * start);

			/**
			 * Constructs a directory entry.
			 * @param name the name of the entry.
			 * @param file_type the type of the entry.
			 * @param fs the filesystem where the directory entry resides.
			 * @param inode the inode on the filesystem where the directory entry is found.
			 * @param file_size the size of the entry.
			 * @param permissions the permissions of the entry, in `chmod` format.
			 * @param owner the owner of the entry.
			 * @param group the group the entry belongs to.
			 */
			direntry(const kstring & name, direntry::type file_type, filesystem * fs,
				unsigned int inode, unsigned long long file_size = 0,
				unsigned short permissions = 00666, unsigned short owner = 0,
				unsigned short group = 0);
			/**
			 * Constructs a directory entry as a copy of another directory entry.
			 * @param entry the entry to copy.
			 */
			direntry(const direntry & entry);

			/**
			 * Sets the pointer to the next directory entry in a list.
			 * @param next the next directory entry.
			 */
			void set_next(direntry * next) { this->next = next; }
			/**
			 * Gets the next directory entry in a list.
			 * @return the next directory entry or `nullptr` if not available.
			 */
			direntry * get_next() const { return next; }

			/**
			 * Gets the name of the entry.
			 * @return the name of the directory entry.
			 */
			const kstring & get_name() const { return name; }
			/**
			 * Gets the type of the entry.
			 * @return the type of the directory entry.
			 */
			direntry::type get_type() const { return entry_type; }
			/**
			 * Gets the size of the entry.
			 * @return the size of the directory entry.
			 */
			unsigned long long get_size() const { return file_size; }
			/**
			 * Gets the inode number of the entry.
			 * @return the inode number of the directory entry.
			 */
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

