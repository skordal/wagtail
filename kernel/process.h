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
	/**
	 * Class representing a process.
	 */
	class process final
	{
		friend class scheduler;

		public:
			/** Default application stack size. */
			static const unsigned int DEFAULT_STACK_SIZE = 16 * 1024;

			/**
			 * Constructs a new process from an executable file.
			 * @todo Remove this constructor and replace it with something better.
			 */
			process(file * app_file);
			~process();

			/**
			 * Gets the stored register contents for this process.
			 * @return the stored register contents for this process.
			 */
			register_contents * get_registers() { return &registers; }

			/**
			 * Gets the application translation table.
			 * @return a pointer to the application translation table.
			 */
			mmu::translation_table<2048> * get_translation_table() const { return translation_table; }

			/**
			 * Gets the process ID. This value is assigned to a process
			 * by the scheduler when it is scheduled.
			 * @return the PID of the process.
			 */
			unsigned int get_pid() const { return pid; }
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

			void set_pid(unsigned int pid) { this->pid = pid; }

			void * program_break;
			unsigned int pid = 0;
			mmu::translation_table<2048> * translation_table;

			register_contents registers;
	};
}

#endif

