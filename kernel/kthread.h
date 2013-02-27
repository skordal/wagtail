// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KTHREAD_H
#define WAGTAIL_KTHREAD_H

#include <functional>

#include "mmu.h"
#include "register_contents.h"
#include "process.h"

#include "asm/syscalls.h"

namespace wagtail
{
	class kthread;

	/**  Entry-point function, used when starting up new kthreads. */
	void kthread_entry_point(kthread *);

	/**
	 * Kernel thread class. These threads run in system mode, where they have
	 * their own stack, but shares the rest of the address space with the kernel.
	 */
	class kthread : public process
	{
		public:
			/**
			 * Destroys a kernel thread.
			 * @warning Undefined behaviour will result if the thread is running when
			 *          the destructor is run!
			 */
			~kthread() { delete[] stack; }

			/**
			 * Runs the kernel thread.
			 */
			virtual int run() = 0;

			/**
			 * Enables the process address space.
			 * For kernel thread, this calls `mmu::set_application_table` with a `nullptr`
			 * argument, which causes the translation of user-space addresses to be disabled
			 * for the thread.
			 */
			virtual void enable_addrspace() override;

			/**
			 * Waits for the completion of an I/O operation.
			 * @param op the operation to wait for.
			 */
			static void wait_for_io(io_operation * op);

			/**
			 * Exits the kernel thread.
			 * @param n exit code.
			 */
			static void exit(int n) __attribute((noreturn));

			/** Yields control to another thread. */
			static void yield();
		protected:
			/** Constructs a kernel thread object. */
			kthread(unsigned int stack_size = DEFAULT_STACK_SIZE);
		private:
			char * stack;
		friend void wagtail::kthread_entry_point(kthread *);
	};
}

#endif

