// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "scheduler.h"

using namespace wagtail;

// Register block used by the context switching code to read/write register
// contents from/to:
extern register_contents * register_block;

scheduler * scheduler::kernel_scheduler = nullptr;

void scheduler::start()
{
	timer::get(0).start(std::bind(&scheduler::interrupt, this), timer::mode::repeating, 1000000);
	while(true) asm volatile("wfi\n\t");
}

scheduler::scheduler()
{
	kernel::message() << "Initializing scheduler:" << kstream::newline;
	placeholder_block = new register_contents;
	register_block = placeholder_block;

	// Use timer0 for context switching at regular intervals:
	timer::get(0).reserve();

	// Set up an initial processes:
	kernel::message() << "Opening test application file..." << kstream::newline;
	file * test_app_file = vfs::get()->open_file("/hello");

	// Create 5 test processes:
	kernel::message() << "Creating test processes..." << kstream::newline;
	for(int i = 0; i < 5; ++i)
	{
		process * new_proc = new process(test_app_file);
		new_proc->set_pid(next_pid++);
		process_queue.push_front(new_proc);
	}

	// Install the handler for the exit syscall:
	syscall_handler::get()->register_handler(std::bind(&scheduler::syscall_exit, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), syscall_number::exit);
}

void * scheduler::syscall_exit(void * retval, void * unused1, void * unused2)
{
	// TODO: Do something about the return value.
	kernel::message() << "Process " << (int) current_process->get_pid() << " exiting." << kstream::newline;

	// Delete the process:
	delete current_process;
	current_process = nullptr;

	// Set the register block to the placeholder to prevent errors when storing 
	// the current context on the next context switch:
	register_block = placeholder_block;

	// Wait for the next context switch:
	while(true) asm volatile("cpsie if\n\twfi\n\t");
}

void scheduler::interrupt()
{
	process * next_process;

	// Push the currently running process back in the queue. If there are no other
	// processes in the queue, it will be run again:
	if(current_process != nullptr)
		process_queue.push_back(current_process);

	// Gets the next process in the queue, or halts when the queue is empty:
	if(!process_queue.pop_front(next_process))
	{
#ifdef WAGTAIL_SCHEDULER_DEBUG
		kernel::message() << "No processes left to run, halting kernel." << kstream::newline;
#endif
		kernel::halt();
	} else {
		current_process = next_process;
		mmu::set_application_table(current_process->get_translation_table(), current_process->get_pid());
		register_block = current_process->get_registers();
#ifdef WAGTAI_SCHEDULER_DEBUG
		kernel::message() << "Scheduling process " << (int) current_process->get_pid() << " to run" << kstream::newline;
#endif
	}
}

