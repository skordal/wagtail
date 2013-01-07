// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_FILESYSTEM_H
#define WAGTAIL_FILESYSTEM_H

#include "direntry.h"
#include "file.h"
#include "kstring.h"
#include "partition.h"

namespace wagtail
{
	class direntry;
	class file;

	/**
	 * Class representing a filesystem on a partition.
	 */
	class filesystem
	{
		static_assert(sizeof(unsigned long long) == 8, "unsigned long long must be 64 bytes long");

		public:
			/**
			 * Initializes a filesystem for the specified partition. This method
			 * can automatically determine which filesystem driver to use for a
			 * partition and instantiate the correct type of filesystem object.
			 * @param part the partition to open a filesystem on.
			 * @param type the MBR filesystem code to use for the filesystem, or `partition::auto_type`
			 *             to automatically detect the filesystem.
			 * @return a filesystem object for the partition, or `nullptr` if an error occurs.
			 */
			static filesystem * initialize(partition * part, char type = partition::auto_type);

			/** Destructs the filesystem and releases the underlying partition. */
			virtual ~filesystem() { if(part != nullptr) part->release(); }

			/**
			 * Gets the read-only status for the filesystem.
			 * @return the read-only status for the filesystem.
			 */
			bool is_read_only() const { return read_only; }
			/**
			 * Sets the read-only status for the filesystem.
			 * @param read_only the new read-only statys.
			 */
			void set_read_only(bool read_only = true) { this->read_only = read_only; }

			/**
			 * Gets the amount of free space on the filesystem.
			 * @return the amount of free space on the filesystem in bytes.
			 */
			virtual unsigned long long get_free_space() const = 0;

			/**
			 * Gets the filesystem label.
			 * @return the filesystem label.
			 */
			virtual const kstring & get_label() const = 0;

			/**
			 * Checks if the filesystem object can be used.
			 * @return `true` if the filesystem can be used for I/O, `false` otherwise.
			 */
			bool is_usable() const { return usable; }

			/**
			 * Opens a file on the filesystem.
			 * @param path the filesystem-relative path of the file to open.
			 * @return a pointer to a file object for the specified path, or `nullptr` if
			 *         no such file exists.
			 */
			virtual file * open_file(const kstring & path) = 0;

			/**
			 * Reads the contents of the specified directory.
			 * @param path the path of the directory to open.
			 * @return a pointer to the first directory entry in the directory, or `nullptr`
			 *         if the specified path was not found.
			 */
			virtual direntry * read_directory(const kstring & path) = 0;

			/**
			 * Checks if the specified file/directory exists.
			 * @return a `direntry` object for the file if it exists, or `nullptr` if not.
			 */
			virtual direntry * file_exists(const kstring & path);

			/**
			 * Gets the partition the filesystem is on.
			 * @return the partition the filesystem is on.
			 */
			partition * get_partition() const { return part; }
		protected:
			/**
			 * Constructs a filesystem for the specified partition.
			 * @param part the partition where the filesystem is located.
			 */
			filesystem(partition * part) : part(part) { if(part != nullptr) part->reserve(); }

			/**
			 * Sets the usable flag.
			 * @param usable the new value of the usable flag.
			 */
			void set_usable(bool usable = true) { this->usable = usable; }
		private:
			bool read_only = true;
			bool usable = false;
			partition * part;
	};
}

#endif

