// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_H
#define WAGTAIL_PROCESS_H

#include "file.h"
#include "kstream.h"
#include "mmu.h"
#include "mm.h"
#include "register_contents.h"
#include "utils.h"

namespace wagtail
{
	class process
	{
		friend class scheduler;

		public:
			static const unsigned int DEFAULT_STACK_SIZE = 16 * 1024;

			process(file * app_file);
			~process();

			register_contents * get_registers() { return &registers; }
		private:
			struct header
			{
				unsigned int entry_point;
				char signature[3];
				char version;
				unsigned int data_start;
				unsigned int bss_start;
				unsigned int application_end;
			} __attribute((packed));

			void * program_break;
			mmu::translation_table<2048> * translation_table;

			register_contents registers;
	};
}

#endif

