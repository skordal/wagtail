// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_FILESYSTEM_H
#define WAGTAIL_FILESYSTEM_H

#include "direntry.h"
#include "kstring.h"
#include "partition.h"

namespace wagtail
{
	class direntry;

	class filesystem
	{
		static_assert(sizeof(unsigned long long) == 8, "unsigned long long must be 64 bytes long");

		public:
			// Initializes a filesystem instance for the specified
			// partition. This method determines which filesystem driver
			// to use and instantiates the correct type of object. If
			// an error occurs, nullptr is returned.
			static filesystem * initialize(partition * part);

			bool is_read_only() const { return read_only; }
			void set_read_only(bool read_only = true) { this->read_only = read_only; }

			// Gets the amount of free space on the filesystem in bytes:
			virtual unsigned long long get_free_space() const = 0;

			// Gets the filesystem label:
			virtual const kstring & get_label() const = 0;

			// Returns whether this filesystem can be used. If a fatal error occurs
			// during initialization of a filesystem driver, usable is set to false:
			bool is_usable() const { return usable; }

			// Reads the specified directory and returns a pointer to the first directory
			// entry for the directory. If there are is no such directory, nullptr is
			// returned.
			virtual direntry * read_directory(const kstring & path) = 0;
		protected:
			filesystem(partition * part) : part(part) {}
			virtual ~filesystem() {}

			// Gets the partition the filesystem is on:
			partition * get_partition() const { return part; }

			// Sets the usable flag:
			void set_usable(bool usable = true) { this->usable = usable; }
		private:
			bool read_only = true;
			bool usable = false;
			partition * part;
	};
}

#endif

