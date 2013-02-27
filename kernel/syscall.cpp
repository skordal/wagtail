// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "syscall.h"

using namespace wagtail;

syscall_handler * syscall_handler::global_syscall_handler = nullptr;

void * handle_syscall(unsigned char syscall, void * arg1, void * arg2, void * arg3)
{
	return syscall_handler::get()->handle_syscall(syscall, arg1, arg2, arg3);
}

void syscall_handler::initialize()
{
	global_syscall_handler = new syscall_handler;
}

void syscall_handler::register_handler(std::function<void *(void *, void *, void *)> handler,
	unsigned char number)
{
	handlers[number] = handler;
}

void syscall_handler::unregister_handler(unsigned char number)
{
	handlers[number] = nullptr;
}

syscall_handler::syscall_handler()
{
	for(unsigned char i = 0; i != 0xff; ++i)
		handlers[i] = nullptr;
}

void * syscall_handler::handle_syscall(unsigned char number, void * arg1, void * arg2, void * arg3)
{
	if(handlers[number] == nullptr)
	{
		invalid_handler(number);
		return (void *) 0;
	} else
		return handlers[number](arg1, arg2, arg3);
}

void syscall_handler::invalid_handler(unsigned char number)
{
	kernel::message() << "Error: invalid syscall: " << (int) number << kstream::newline;
}

