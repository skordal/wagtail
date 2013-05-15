// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "process.h"
#include "scheduler.h"

using namespace wagtail;

process::~process()
{
	// Unmap the code and data sections:
	if(code_pages > 0)
		translation_table->unmap_interval((void *) 0x1000, program_break);

	// Unmap the stack:
	if(stack_pages > 0)
		translation_table->unmap_interval((void *) (0x80000000U - DEFAULT_STACK_SIZE), (void *) 0x80000000);

	delete translation_table;
}

process::process() : code_pages(0), data_pages(0), stack_pages(0)
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

