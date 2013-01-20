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
#include "waitblock.h"

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

			/** Gets a process ID for a new process. */
			unsigned int allocate_pid();

			/**
			 * Gets a process from the process queue.
			 * @param pid the PID to search for.
			 * @return the process corresponding to the PID.
			 */
			process * get_process(unsigned int pid) const;

			/**
			 * Gets a pointer to the currently running process.
			 * @return the currently running process, or `nullptr` if none.
			 */
			process * get_current_process() const { return current_process; }
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

			/**
			 * Handler for the fork syscall. This forks the currently running process.
			 * @param unused1 unused system call parameter.
			 * @param unused2 unused system call parameter.
			 * @param unused3 unused system call parameter.
			 * @return the PID of the child process for the parent, or 0 for the child process.
			 */
			void * syscall_fork(void * unused1, void * unused2, void * unused3);

			/**
			 * Handler for the wait syscall. This causes the current process to block until
			 * the specified process has terminated. The exit code is then returned to the
			 * process.
			 * @param pid the PID of the process to wait for.
			 * @param status_loc a memory location to write the status code to.
			 * @param unused1 unused system call parameter.
			 */
			void * syscall_wait(void * pid, void * status_loc, void * unused1) __attribute((noreturn));

			/**  Scheduler interrupt handler. */
			void interrupt();

			kqueue<process *> process_queue;
			process * current_process = nullptr;

			unsigned int next_pid = 2;
			static scheduler * kernel_scheduler;
	};
}

#endif

