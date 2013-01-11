// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SYSCALL_H
#define WAGTAIL_SYSCALL_H

#include <functional>

/**
 * Syscall handler function. This method is called by the interrupt handler
 * in `interrupts.S`. 
 * @param syscall the syscall number.
 * @param arg1 the first syscall argument.
 * @param arg2 the second syscall argument.
 * @param arg3 the third syscall argument.
 * @return the return value for the syscall, casted to a `void *`.
 */
extern "C" void * handle_syscall(unsigned char syscall, void * arg1, void * arg2,
	void * arg3);

namespace wagtail
{
	/**
	 * System call handler.
	 */
	class syscall_handler final
	{
		friend void * ::handle_syscall(unsigned char, void *, void *, void *);

		public:
			/** Initializes the system call handler instance. */
			static void initialize();
			/**
			 * Gets the global system call handler instance.
			 * @return the global system call handler instance.
			 */
			static syscall_handler * get() { return global_syscall_handler; }

			/**
			 * Registers a handler for a system call.
			 * @param handler the handler function.
			 * @param number the system call number to use `handler` for.
			 */
			void register_handler(std::function<void *(void *, void *, void *)> handler,
				unsigned char number);
			/**
			 * Removes a registered system call handler.
			 * @param number the system call number to remove the handler from.
			 */
			void unregister_handler(int number);
		private:
			syscall_handler();

			void * handle_syscall(unsigned char number, void * arg1, void * arg2, void * arg3);
			void invalid_handler(unsigned char number);

			std::function<void *(void *, void *, void *)> handlers[256];

			static syscall_handler * global_syscall_handler;
	};
}

#endif

