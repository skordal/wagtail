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

	// A file object.
	class file
	{
		public:
			file(const kstring & path, filesystem * fs, unsigned long long size,
				unsigned int start_inode, bool read_only = true);
			virtual ~file() {}

			virtual bool read(void * buffer, unsigned int length, unsigned int offset = 0) = 0;
			virtual bool write(void * buffer, unsigned int length, unsigned int offset = 0) = 0;

			virtual unsigned long long get_size() const { return size; }

			bool is_read_only() const { return read_only; }
			const kstring & get_path() const { return path; }
		protected:
			void set_read_only(bool ro = true) { read_only = ro; }
			unsigned int get_start_inode() const { return start_inode; }
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

