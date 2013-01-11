// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "scheduler.h"

using namespace wagtail;

extern register_contents * register_block;

scheduler * scheduler::kernel_scheduler = nullptr;

void scheduler::start()
{
	timer::get(0).start(std::bind(&scheduler::interrupt, this), timer::mode::repeating, 1000000);
	while(running) asm volatile("wfi\n\t");
}

scheduler::scheduler()
{
	kernel::message() << "Initializing scheduler:" << kstream::newline;
	register_block = new register_contents;

	// Use timer0 for context switching at regular intervals:
	timer::get(0).reserve();

	// Set up an initial process:
	kernel::message() << "Opening test application file..." << kstream::newline;
	file * test_app_file = vfs::get()->open_file("/hello");
	kernel::message() << "Creating test process..." << kstream::newline;
	test_process = new process(test_app_file);
}

void scheduler::interrupt()
{
	kernel::message() << "Scheduler interrupt!" << kstream::newline;
	kernel::message() << *register_block << kstream::newline;
	register_block = scheduler::get()->test_process->get_registers();
}

