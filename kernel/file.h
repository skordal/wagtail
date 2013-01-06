// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_FILE_H
#define WAGTAIL_FILE_H

#include "filesystem.h"
#include "kstring.h"

namespace wagtail
{
	class filesystem;

	/**
	 * Class representing a file. It also allows for I/O operations on the file, and must
	 * be overridden by filesystem specific file classes to allow for this.
	 */
	class file
	{
		public:
			/**
			 * Constructs a file object.
			 * @param path the path to the file, relative to the filesystem.
			 * @param fs the filesystem the file is located on.
			 * @param size the size of the file.
			 * @param start_inode the first inode of the file.
			 * @param read_only specifies if the file is read-only or not.
			 */
			file(const kstring & path, filesystem * fs, unsigned long long size,
				unsigned int start_inode, bool read_only = true);
			/** Destucts a file object. */
			virtual ~file() {}

			/**
			 * Reads from a file.
			 * @param buffer the target buffer.
			 * @param length the number of bytes to read.
			 * @param offset the offset in the file to start reading from.
			 * @return `true` if successful, `false` otherwise.
			 */
			virtual bool read(void * buffer, unsigned int length, unsigned int offset = 0) = 0;

			/**
			 * Writes to a file.
			 * @param buffer the buffer to read data from.
			 * @param length the length of the buffer.
			 * @param offset the offset into the file to start writing at.
			 * @return `true` if successful, `false` otherwise.
			 */
			virtual bool write(void * buffer, unsigned int length, unsigned int offset = 0) = 0;

			/**
			 * Gets the size of the file.
			 * @return the size of the file.
			 */
			virtual unsigned long long get_size() const { return size; }

			/**
			 * Returns the read-only status of the file.
			 * @return `true` if the file is read-only, `false` otherwise.
			 */
			bool is_read_only() const { return read_only; }
			/**
			 * Gets the filesystem-relative path of the file.
			 * @return the filesystem relative path of the file.
			 */
			const kstring & get_path() const { return path; }
		protected:
			/**
			 * Sets the read-only status of the file.
			 * @param ro the new read-only status.
			 */
			void set_read_only(bool ro = true) { read_only = ro; }
			/**
			 * Gets the start inode of the file.
			 * @return the start inode of the file.
			 */
			unsigned int get_start_inode() const { return start_inode; }
			/**
			 * Gets the filesystem the file resides on.
			 * @return the filesystem where the file resides.
			 */
			filesystem * get_filesystem() const { return fs; }
		private:
			kstring path;

			filesystem * fs;

			unsigned long long size;
			unsigned int start_inode;

			bool read_only;
	};
}

#endif

