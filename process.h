// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_H
#define WAGTAIL_PROCESS_H

#include "kernel.h"
#include "io.h"
#include "mmu.h"

namespace wagtail
{
	class process
	{
		public:
			// Creates a new process from the specified memory buffer.
			// The buffer may be freed afterwards.
			process(void * buffer);
		private:
			mmu::application_translation_table_t * translation_table;
	};
}

#endif

