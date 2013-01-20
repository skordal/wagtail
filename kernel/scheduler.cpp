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
	timer::get(0).start(std::bind(&scheduler::interrupt, this), timer::mode::repeating, 1000000);
	while(true) asm volatile("wfi\n\t");
}

unsigned int scheduler::allocate_pid()
{
	unsigned int retval = next_pid++;

	// Check if the PID is unused:
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

	// Use timer0 for context switching at regular intervals:
	timer::get(0).reserve();

	// Set up an initial processes:
	kernel::message() << "Starting /sbin/init..." << kstream::newline;
	file * init_file = vfs::get()->open_file("/proctest");
	if(init_file == nullptr)
	{
		kernel::message() << "*** Could not find /sbin/init! ***" << kstream::newline;
		kernel::panic();
	}

	char * init_buffer = new char[init_file->get_size()];
	if(!init_file->read(init_buffer, init_file->get_size()))
	{
		kernel::message() << "*** Could not read /sbin/init! ***" << kstream::newline;
		kernel::panic();
	}

	process * init_proc = new process(init_buffer);
	if(!init_proc->is_usable())
	{
		kernel::message() << "*** Could not execute /sbin/init! ***" << kstream::newline;
		kernel::panic();
	}

	// Schedule the first process:
	process_queue.push_front(init_proc);

	// Clean up:
	delete[] init_buffer;

	// Install the handler for the exit syscall:
	syscall_handler::get()->register_handler(std::bind(&scheduler::syscall_exit, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), SYSCALL_EXIT);
	// Install the handler for the fork syscall:
	syscall_handler::get()->register_handler(std::bind(&scheduler::syscall_fork, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), SYSCALL_FORK);
	// Install the handler for the wait syscall:
	syscall_handler::get()->register_handler(std::bind(&scheduler::syscall_wait, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), SYSCALL_WAIT);
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

	// Wait for the next context switch:
	while(true) asm volatile("cpsie if\n\twfi\n\t");
}

void * scheduler::syscall_fork(void * unused1, void * unused2, void * unused3)
{
#ifdef WAGTAIL_SCHEDULER_DEBUG
	if(current_process == nullptr)
	{
		kernel::message() << "Error: cannot fork process, none is currently active!" << kstream::newline;
		kernel::panic();
	}
#endif

	process * child = current_process->fork();
	mmu::set_application_table(current_process->get_translation_table(), current_process->get_pid());

	if(child->is_usable())
	{
		kernel::message() << "New process: " << (int) child->get_pid() << kstream::newline;
		process_queue.push_back(child);
		return (void *) child->get_pid();
	} else {
#ifdef WAGTAIL_SCHEDULER_DEBUG
		kernel::message() << "Error: forking process " << (int) current_process->get_pid() << "failed!"
			<< kstream::newline;
#endif

		delete child;
		return (void *) -1;
	}
}

void * scheduler::syscall_wait(void * pid, void * status_loc, void * unused1)
{
#ifdef WAGTAIL_SCHEDULER_DEBUG
	kernel::message() << "Process " << (int) current_process->get_pid()
		<< ": waiting for process " << (int) pid << kstream::newline;
#endif

	current_process->set_block(new waitblock((unsigned int) pid, (unsigned int *) status_loc,
		current_process));
	process_queue.push_back(current_process);

	current_process = nullptr;
	register_block = nullptr;

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
		timer::get(0).stop();
		timer::get(0).release();
#ifdef WAGTAIL_SCHEDULER_DEBUG
		kernel::message() << "No processes left to run, halting kernel." << kstream::newline;
#endif
		kernel::halt();
	} else {
		current_process = next_process;
		mmu::set_application_table(current_process->get_translation_table(), current_process->get_pid());

		if(current_process->is_blocking())
		{
			// TODO: Schedule another process if the chosen one is blocking.
#ifdef WAGTAIL_SCHEDULER_DEBUG
			kernel::message() << "Process blocking" << kstream::newline;
#endif
			process_queue.push_back(current_process);
			current_process = nullptr;
			return;
		} else if(!current_process->is_blocking() && current_process->get_block() != nullptr)
		{
#ifdef WAGTAIL_SCHEDULER_DEBUG
			kernel::message() << "Process block released" << kstream::newline;
#endif
			current_process->get_block()->block_return();
			delete current_process->get_block();
			current_process->set_block(nullptr);
		}

		register_block = &current_process->get_registers();
#ifdef WAGTAIL_SCHEDULER_DEBUG
		kernel::message() << "Scheduling process " << (int) current_process->get_pid() << " to run" << kstream::newline;
#endif
		kernel::message() << current_process->get_registers() << kstream::newline;
	}
}

