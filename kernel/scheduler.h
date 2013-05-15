// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SCHEDULER_H
#define WAGTAIL_SCHEDULER_H

#include <functional>

#include "kqueue.h"
#include "process.h"
#include "register_contents.h"
#include "syscall.h"
#include "timer.h"

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
			 * Schedules a process.
			 * @param proc the process to schedule.
			 */
			void schedule(process * proc) { process_queue.push_back(proc); }

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

			/**
			 * Gets the scheduler queue length.
			 * @return the length of the process queue.
			 */
			int get_queue_length() const { return process_queue.get_length(); }
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

			unsigned int next_pid = 2;
			static scheduler * kernel_scheduler;
	};
}

#endif

