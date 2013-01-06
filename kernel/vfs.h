// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_VFS_H
#define WAGTAIL_VFS_H

#include "direntry.h"
#include "file.h"
#include "filesystem.h"
#include "kstring.h"
#include "partition.h"

namespace wagtail
{
	/**
	 * Virtual filesystem interface.
	 */
	class vfs final : public filesystem
	{
		public:
			/** Initializes the virtual filesystem. */
			static void initialize();
			/** Gets the global virtual filesystem interface. */
			static vfs * get() { return vfs_interface; }

			// Mounts a partition at the specified mountpoint. The directory
			// must exist, unless it is /. If part_type is partition::auto_type,
			// filesystem::initialize is used to guess the filesystem type.
			// If an error occurs, nullptr is returned, if not, the mounted
			// filesystem is returned.
			/**
			 * Mounts a partition at the specified mountpoint.
			 * @param path the path to mount the partition at.
			 * @param part the partition to mount.
			 * @param part_type the type of the partition to mount.
			 * @return a pointer to the mounted filesystem object or `nullptr` if
			 *         an error occured.
			 */
			filesystem * mount(const kstring & path, partition * part,
				char part_type = partition::auto_type);
			/**
			 * Unmounts a filesystem.
			 * @param mountpoint the mountpoint the unmount a filesystem from.
			 */
			void unmount(const kstring & mountpoint);

			/**
			 * Gets the total free space in the virtual filesystem.
			 * @return the total free space in all filesystems combined.
			 */
			unsigned long long get_free_space() const override;

			/**
			 * Opens a file on a filesystem.
			 * @param path path to the file to open.
			 * @return a pointer to the opened file object, or `nullptr` if the file
			 *         cannot be found or an error occured.
			 */
			file * open_file(const kstring & path) override;

			/**
			 * Reads a directory on a filesystem.
			 * @param path path to the directory to read.
			 * @return the first directory entry in the directory or `nullptr` if the
			 *         directory does not exist.
			 */
			direntry * read_directory(const kstring & path) override;

			/**
			 * Gets the filesystem mounted at the specified path.
			 * @return the filesystem mounted at the specified path, or `nullptr` if none.
			 */
			filesystem * get_filesystem(const kstring & path) const;

			/**
			 * Prints a list of the mounted filesystems. This method is intended for debug
			 * use only.
			 */
			void print_mountlist();
		private:
			vfs();

			const kstring & get_label() const override { return vfs_label; }

			// Class containing information about a mount point:
			class mountpoint final
			{
				public:
					mountpoint(const kstring & path, filesystem * fs)
						: path(path), fs(fs) {}
					~mountpoint() {}

					void set_next(mountpoint * next) { this->next = next; }
					mountpoint * get_next() const { return next; }

					void set_prev(mountpoint * prev) { this->prev = prev; }
					mountpoint * get_prev() const { return prev; }

					const kstring & get_path() const { return path; }
					filesystem * get_filesystem() const { return fs; }
				private:
					kstring path;
					filesystem * fs;
					mountpoint * next = nullptr, * prev = nullptr;
			};

			// Adds a mountpoint to the mountpoint list:
			void add_mountpoint(mountpoint * m);

			// List for maintaining the mounted filesystems, sorted with the longest
			// path name first:
			mountpoint * mount_list = nullptr;

			kstring vfs_label;
			static vfs * vfs_interface;
	};
};

#endif

