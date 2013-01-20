// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "process.h"
#include "scheduler.h"

using namespace wagtail;

process::process(char * buffer) : process()
{
	// Get the application header:
	header * app_header = reinterpret_cast<header *>(buffer);

	// Calculate how many pages to allocate:
	code_pages = (app_header->data_start - 0x1000U) >> 12;
	data_pages = (((app_header->application_end + 4095) & utils::mask_left(4096))
		- (code_pages << 12) - 0x1000) >> 12;
	stack_pages = DEFAULT_STACK_SIZE >> 12;

	// Setup the application address space:
	setup_addrspace();

	// Set the application stack pointer:
	registers.r[13] = 0x80000000;

	// Set the application as the current one:
	mmu::set_application_table(translation_table, pid);

	// Copy the application data into the mapped address space:
	for(unsigned int i = 0; i < app_header->data_start - 0x1000U; ++i)
		*((char *) 0x1000 + i) = buffer[i];
	for(unsigned int i = 0; i < app_header->bss_start - app_header->data_start; ++i)
		*((char *) 0x1000 + (code_pages * 0x1000) + i) = buffer[i + (code_pages * 0x1000)];

	usable = true;
}

process::~process()
{
	// TODO: Terminate child processes here!

	// Call all termination listeners:
	for(auto listener : termination_listeners)
		listener->process_terminated(this);

	// Unmap the code and data sections:
	translation_table->unmap_interval((void *) 0x1000, program_break);

	// Unmap the stack:
	translation_table->unmap_interval((void *) (0x80000000U - DEFAULT_STACK_SIZE), (void *) 0x80000000);

	delete translation_table;
}

process * process::fork()
{
	process * child = new process(this);
	child->add_termination_listener(this);
	children.push_front(child);

	// Fork returns 0 to the child process:
	child->get_registers().r[0] = 0;

	return child;
}

void process::add_termination_listener(termination_listener * listener)
{
	termination_listeners.push_front(listener);
}

void process::remove_termination_listener(termination_listener * listener)
{
	for(auto i = termination_listeners.begin(); i != termination_listeners.end(); ++i)
	{
		if((*i) == listener)
		{
			termination_listeners.remove(i);
			break;
		}
	}
}

void process::set_child_termination_listener(termination_listener * listener)
{
	child_termination_listener = listener;
}

process::process() : termination_listener()
{
	translation_table = new (8192) mmu::translation_table<2048>();
	translation_table->clear();

	// If this is the first process created, the scheduler may be in the process of being
	// constructed. If that is the case, assign PID 1:
	if(scheduler::get() == nullptr)
		pid = 1;
	else
		pid = scheduler::get()->allocate_pid();
}

process::process(process * parent) : process()
{
	this->parent = parent;

	translation_table = new (8192) mmu::translation_table<2048>();
	translation_table->clear();

	registers = register_contents(parent->get_registers());

	// Set up the address space for the process:
	code_pages = parent->code_pages;
	data_pages = parent->data_pages;
	stack_pages = parent->stack_pages;
	setup_addrspace();

	// Very inefficiently copy the process space page-by-page:
	char * buffer = new char[4096];

	unsigned int current_address = 0x1000U;

	// Copy the .text and .data sections:
	for(unsigned int i = 0; i < code_pages + data_pages; ++i, current_address += 4096)
	{
		mmu::set_application_table(parent->get_translation_table(), parent->get_pid());
		for(int i = 0; i < 4096; ++i)
			buffer[i] = *((volatile char *) (current_address + i));
		mmu::set_application_table(translation_table, get_pid());
		for(int i = 0; i < 4096; ++i)
			*((volatile char *) (current_address + i)) = buffer[i];
	}

	// Copy the stack:
	current_address = 0x80000000U - (stack_pages * 4096);
	for(unsigned int i = 0; i < stack_pages; ++i, current_address += 4096)
	{
		mmu::set_application_table(parent->get_translation_table(), parent->get_pid());
		for(int i = 0; i < 4096; ++i)
			buffer[i] = *((volatile char *) (current_address + i));
		mmu::set_application_table(translation_table, get_pid());
		for(int i = 0; i < 4096; ++i)
			*((volatile char *) (current_address + i)) = buffer[i];
	}

	usable = true;
}

void process::setup_addrspace()
{
	// Start allocating pages for the process:
	unsigned int current_address = 0x1000U;

	// Set up the code (.text) section:
	for(unsigned int i = 0; i < code_pages; ++i, current_address += 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RO, mmu::CODE, false);
	}

	// Set up the data (.data + .bss) section:
	for(unsigned int i = 0; i < data_pages; ++i, current_address += 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RW, mmu::DATA, false);
	}

	// Set up the stack:
	current_address = 0x80000000U - 4096U;
	for(unsigned int i = 0; i < stack_pages; ++i, current_address -= 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RW, mmu::STACK, false);
	}

	// Set the value of the initial program break:
	program_break = reinterpret_cast<void *>(0x1000 + (code_pages * 0x1000) + (data_pages * 0x1000));
}

void process::process_terminated(process * child)
{
	kernel::message() << "Process " << (int) pid << ": child terminated: " << (int) child->get_pid()
		<< kstream::newline;
	if(child_termination_listener != nullptr)
		child_termination_listener->process_terminated(child);

	// TODO: Send SIGCHLD.
	for(auto i = children.begin(); i != children.end(); ++i)
	{
		if((*i) == child)
		{
			children.remove(i);
			break;
		}
	}
}

