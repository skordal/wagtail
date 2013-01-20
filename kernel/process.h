// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_H
#define WAGTAIL_PROCESS_H

#include "file.h"
#include "kqueue.h"
#include "kstream.h"
#include "mmu.h"
#include "mm.h"
#include "process_block.h"
#include "register_contents.h"
#include "termination_listener.h"
#include "utils.h"

namespace wagtail
{
	/**
	 * Class representing a process.
	 */
	class process final : private termination_listener
	{
		friend class scheduler;

		public:
			/** Default application stack size. */
			static const unsigned int DEFAULT_STACK_SIZE = 16 * 1024;

			/**
			 * Constructs a new process from a buffer containing an executable file.
			 * @param buffer the buffer containing the executable file.
			 */
			process(char * buffer);

			/** Process destructor, returns allocated memory to the memory pool. */
			virtual ~process();

			/**
			 * Forks this process, returning the child process.
			 */
			process * fork();

			/**
			 * Gets the stored register contents for this process.
			 * @return the stored register contents for this process.
			 */
			register_contents & get_registers() { return registers; }

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
			 * Adds a termination listener to this process.
			 * @param listener pointer to the listener to add.
			 */
			void add_termination_listener(termination_listener * listener);

			/**
			 * Removes a termination listener from this process.
			 * @param listener pointer to the listener to remove.
			 */
			void remove_termination_listener(termination_listener * listener);

			/**
			 * Sets a termination listener that listens for child processes.
			 * @param listener pointer to the listener to set.
			 */
			void set_child_termination_listener(termination_listener * listener);

			/**
			 * Sets the current process block.
			 * @param block the block to use for blocking.
			 */
			void set_block(process_block * block) { this->block = block; }

			/**
			 * Gets the current process block.
			 * @return the currently active process block.
			 */
			process_block * get_block() const { return block; }

			/**
			 * Returns `true` if the process has been successfully loaded and initialized.
			 * @return `true` if the process object can be used, `false` otherwise.
			 */
			bool is_usable() const { return usable; }

			/**
			 * Returns `true` if the process is currently blocking.
			 * @return `true` if the process is blocking, `false` otherwise.
			 */
			bool is_blocking() const { return !(block == nullptr || !block->is_active()); }

			/**
			 * Gets the process exit code.
			 * @return the process exit code.
			 */
			unsigned int get_exit_code() const { return exit_code; }

			/**
			 * Gets the number of children of the process.
			 * @return the number of children of the process.
			 */
			unsigned int get_num_children() const { return children.get_length(); }
		private:
			/** Common constructor. */
			process();

			/** Constructor used when forking a process. */
			process(process * parent);

			/**
			 * Allocates memory for the process address space. Used during construction of the process object.
			 * @warning `code_pages`, `data_pages` and `stack_pages` must be set before calling this method.
			 */
			void setup_addrspace();

			/** Listener for child process terminations. */
			void process_terminated(process * child) override;

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
			bool usable = false;
			unsigned int exit_code = 0;

			process * parent = nullptr;
			process_block * block = nullptr;

			kqueue<process *> children;
			kqueue<termination_listener *> termination_listeners;
			termination_listener * child_termination_listener = nullptr;

			register_contents registers;

			// Numbers of allocated pages for the various process sections:
			unsigned int code_pages, data_pages, stack_pages;
	};
}

#endif

