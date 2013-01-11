// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "syscall.h"

using namespace wagtail;

syscall_handler * syscall_handler::global_syscall_handler = nullptr;

void * handle_syscall(unsigned char syscall, void * arg1, void * arg2, void * arg3)
{
	kernel::message() << "Syscall: " << (int) syscall << kstream::newline;
	return syscall_handler::get()->handle_syscall((syscall_number) syscall, arg1, arg2, arg3);
}

void syscall_handler::initialize()
{
	global_syscall_handler = new syscall_handler;
}

void syscall_handler::register_handler(std::function<void *(void *, void *, void *)> handler,
	syscall_number number)
{
	handlers[(unsigned char) number] = handler;
}

void syscall_handler::unregister_handler(syscall_number number)
{
	handlers[(unsigned char) number] = nullptr;
}

syscall_handler::syscall_handler()
{
	for(unsigned char i = 0; i != 0xff; ++i)
		handlers[i] = nullptr;
}

void * syscall_handler::handle_syscall(syscall_number number, void * arg1, void * arg2, void * arg3)
{
	if(handlers[(unsigned char) number] == nullptr)
	{
		invalid_handler(number);
		return (void *) 0;
	} else
		return handlers[(unsigned char) number](arg1, arg2, arg3);
}

void syscall_handler::invalid_handler(syscall_number number)
{
	kernel::message() << "Error: invalid syscall: " << (int) number << kstream::newline;
}

