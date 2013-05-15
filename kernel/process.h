// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_H
#define WAGTAIL_PROCESS_H

#include "kqueue.h"
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
	class process
	{
		friend class scheduler;

		public:
			/** Default application stack size. */
			static const unsigned int DEFAULT_STACK_SIZE = 16 * 1024;

			/** Process destructor, returns allocated memory to the memory pool. */
			virtual ~process();

			/**
			 * Gets the stored register contents for this process.
			 * @return the stored register contents for this process.
			 */
			register_contents & get_registers() { return registers; }

			/**
			 * Sets the process's translation table as the current one.
			 */
			virtual void enable_addrspace() = 0;

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

			/**
			 * Returns `true` if the process has been successfully loaded and initialized.
			 * @return `true` if the process object can be used, `false` otherwise.
			 */
			bool is_usable() const { return usable; }

			/**
			 * Gets the process exit code.
			 * @return the process exit code.
			 */
			unsigned int get_exit_code() const { return exit_code; }
		protected:
			/** Common constructor. */
			process();

			bool usable = false;
		private:
			/**
			 * Allocates memory for the process address space. Used during construction of the process object.
			 * @warning `code_pages`, `data_pages` and `stack_pages` must be set before calling this method.
			 */
			void setup_addrspace();

			/**
			 * Sets the process exit code, which is passed to parents when terminating.
			 * @param exit_code the process exit code.
			 */
			void set_exit_code(unsigned int exit_code) { this->exit_code = exit_code; }

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
			unsigned int pid;
			mmu::translation_table<2048> * translation_table;
			unsigned int exit_code = 0;

			register_contents registers;

			// Numbers of allocated pages for the various process sections:
			unsigned int code_pages = 0, data_pages = 0, stack_pages = 0;
	};
}

#endif

