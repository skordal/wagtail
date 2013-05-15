// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "scheduler.h"

using namespace wagtail;

// Register block used by the context switching code to read/write register
// contents from/to:
extern volatile register_contents * register_block;

scheduler * scheduler::kernel_scheduler = nullptr;

void scheduler::start()
{
	timer::get(0).start(std::bind(&scheduler::interrupt, this), timer::mode::repeating, 10000);
	while(true) asm volatile("wfi\n\t");
}

unsigned int scheduler::allocate_pid()
{
	unsigned int retval = next_pid++;

	// Check if the PID is in use:
	for(process * proc : process_queue)
		// If the PID exists, try again:
		if(retval == proc->get_pid())
			return allocate_pid();

	return retval;
}

process * scheduler::get_process(unsigned int pid) const
{
	for(auto proc : process_queue)
	{
		if(proc->get_pid() == pid)
			return proc;
	}

	return nullptr;
}

scheduler::scheduler()
{
	kernel::message() << "Initializing scheduler..." << kstream::newline;
	register_block = nullptr;

	// Install the handler for the exit syscall:
	syscall_handler::get()->register_handler(std::bind(&scheduler::syscall_exit, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), SYSCALL_EXIT);
}

void * scheduler::syscall_exit(void * retval, void * unused1, void * unused2)
{
#ifdef WAGTAIL_SCHEDULER_DEBUG
	kernel::message() << "Process " << (int) current_process->get_pid() << " exiting with status code "
		<< (int) retval << kstream::newline;
#endif

	// Delete the process:
	current_process->set_exit_code((int) retval);
	delete current_process;

	current_process = nullptr;
	register_block = nullptr;

	// Re-enable interrupts and wait for the next scheduler interrupt:
	while(true) asm volatile("cpsie if\n\twfi\n\t");
}

void scheduler::interrupt()
{
	process * next_process;
#ifdef WAGTAIL_SCHEDULER_DEBUG
	kernel::message() << "Scheduler interrupt!" << kstream::newline;
#endif

	// Push the currently running process back in the queue. If there are no other
	// processes in the queue, it will be run again:
	if(current_process != nullptr)
		process_queue.push_back(current_process);

	// Gets the next process in the queue, or halts when the queue is empty:
	if(!process_queue.pop_front(next_process))
	{
		timer::get(0).stop();
		kernel::message() << "No processes left to run, halting kernel." << kstream::newline;
		kernel::halt();
	} else {
		current_process = next_process;

		// Switch translation tables:
		current_process->enable_addrspace();

		register_block = &current_process->get_registers();
#ifdef WAGTAIL_SCHEDULER_DEBUG
		kernel::message() << "Scheduling process " << (int) current_process->get_pid() << " to run" << kstream::newline;
#endif
	}
}

