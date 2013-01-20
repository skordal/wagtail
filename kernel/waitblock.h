// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_WAITBLOCK_H
#define WAGTAIL_WAITBLOCK_H

#include "mmu.h"
#include "process.h"
#include "process_block.h"
#include "scheduler.h"
#include "termination_listener.h"

namespace wagtail
{
	/**
	 * Block that waits for a process to terminate.
	 * @todo Support waiting for process groups.
	 */
	class waitblock final : public process_block, public termination_listener
	{
		public:
			/**
			 * Constructs a block that waits for a process to terminate.
			 * @param pid the PID of the process to wait for, -1 to wait for any
			 *            child process.
			 * @param status_loc a location in the calling process's address space
			 *                   where the return value of the terminating process
			 *                   is written.
			 * @param caller a pointer to the calling process.
			 */
			waitblock(unsigned int pid, unsigned int * status_loc, process * caller);
			/** Destructs a waitblock object. */
			~waitblock();

			/**
			 * Checks whether the block is active.
			 * @return `true` if the block is active, `false` otherwise.
			 */
			bool is_active() const override { return active; }

			/**
			 * Sets register values and writes to memory.
			 */
			void block_return() override;

			/**
			 * Callback for the process being waited on.
			 * @param terminated the terminated process.
			 */
			void process_terminated(process * terminated) override;
		private:
			unsigned int waitpid, * status_loc, exit_code;
			process * caller;

			unsigned int terminated_pid;

			bool active = true;
	};
}

#endif

