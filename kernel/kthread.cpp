// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "kthread.h"

using namespace wagtail;

void wagtail::kthread_entry_point(kthread * t)
{
	kthread::exit(t->run());
}

kthread::kthread(unsigned int stack_size) : process()
{
	stack = new (8) char[stack_size];

	// Set the argument to kthread_entry_point:
	get_registers().r[0] = reinterpret_cast<unsigned int>(this);
	// Set the kernel thread stack:
	get_registers().r[13] = reinterpret_cast<unsigned int>(stack) + stack_size;
	// Set the SPSR to switch to system and thumb mode:
	get_registers().spsr = PSR_T|PROCESSOR_MODE_SYS;
	// Set the entry point address:
	get_registers().return_address = (unsigned int) kthread_entry_point + 4;

	usable = true;
}

void kthread::enable_addrspace()
{
	mmu::set_application_table(nullptr, 0);
}

void kthread::exit(int n)
{
	asm volatile(
		"mov r0, %[exit_code]\n\t"
		"svc %[exit_syscall]\n\t"
		:: [exit_code] "r" (n), [exit_syscall] "I" (SYSCALL_EXIT) : "r0");
	while(true) asm volatile("wfi");
}

