// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SCHEDULER_H
#define WAGTAIL_SCHEDULER_H

#include <functional>

#include "file.h"
#include "kqueue.h"
#include "process.h"
#include "register_contents.h"
#include "syscall.h"
#include "timer.h"
#include "vfs.h"

// Define the following to enable scheduler debugging messages to be printed:
#define WAGTAIL_SCHEDULER_DEBUG

namespace wagtail
{
	/**
	 * The scheduler class. The scheduler takes care of switching between
	 * the active user-mode processes. It uses `timer0` to generate a regular
	 * interrupt for the context switching.
	 */
	class scheduler final
	{
		public:
			/** Initializes the scheduler. */
			static void initialize() { kernel_scheduler = new scheduler; }

			/**
			 * Gets the scheduler instance.
			 * @return the global scheduler instance.
			 */
			static scheduler * get() { return kernel_scheduler; }

			/** Starts the scheduler. */
			void start() __attribute((noreturn));
		private:
			scheduler();

			/**
			 * Handler for the exit syscall. This system call terminates the currently running
			 * process and waits for the next context switch to run the next process.
			 * @param retval application exit code.
			 * @param unused1 unused system call parameter.
			 * @param unused2 unused system call parameter.
			 * @return nothing (`nullptr`).
			 */
			void * syscall_exit(void * retval, void * unused1, void * unused2) __attribute((noreturn));

			/**  Scheduler interrupt handler. */
			void interrupt();

			kqueue<process *> process_queue;
			process * current_process = nullptr;

			/** Dummy register block used when no process is running. */
			register_contents * placeholder_block;

			unsigned int next_pid = 1;
			static scheduler * kernel_scheduler;
	};
}

#endif

