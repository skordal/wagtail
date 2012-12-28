// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SYSCALL_H
#define WAGTAIL_SYSCALL_H

#include <functional>

// Syscall handler used by the interrupt handler:
extern "C" void * handle_syscall(unsigned char syscall, void * arg1, void * arg2,
	void * arg3, void * arg4);

namespace wagtail
{
	class syscall_handler final
	{
		friend void * ::handle_syscall(unsigned char, void *, void *, void *, void *);

		public:
			static void initialize();
			static syscall_handler * get() { return global_syscall_handler; }

			// Registers a handler for the specified system call number:
			void register_handler(std::function<void *(void *, void *, void *, void *)> handler,
				unsigned char number);
			// Removes the registered handler for a specified system call number:
			void unregister_handler(int number);
		private:
			syscall_handler();

			void * handle_syscall(unsigned char number, void * arg1, void * arg2, void * arg3, void * arg4);
			void invalid_handler(unsigned char number);

			std::function<void *(void *, void *, void *, void *)> handlers[256];

			static syscall_handler * global_syscall_handler;
	};
}

#endif

