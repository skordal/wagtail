// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "process.h"

using namespace wagtail;

process::process(file * app_file)
{
	translation_table = new (8192) mmu::translation_table<2048>();
	translation_table->clear();

	char * buffer = new char[app_file->get_size()];

	if(!app_file->read(buffer, app_file->get_size()))
	{
		kernel::message() << "Cannot read application file!" << kstream::newline;
		delete[] buffer;
		return; // TODO: handle errors later.
	}

	header * app_header = reinterpret_cast<header *>(buffer);
	kernel::message() << "Application header signature: "
		<< app_header->signature[0] << app_header->signature[1] << app_header->signature[2] << kstream::newline;

	unsigned int app_pages = (app_header->data_start - 0x1000U) >> 12;
	unsigned int data_pages = (((app_header->application_end + 4095) & utils::mask_left(4096))
		- (app_pages << 12) - 0x1000) >> 12;
	unsigned int stack_pages = DEFAULT_STACK_SIZE >> 12;

	kernel::message() << "Application pages: " << (void *) app_pages << kstream::newline;
	kernel::message() << "Data pages: " << (void *) data_pages << kstream::newline;
	kernel::message() << "Stack pages: " << (void *) (DEFAULT_STACK_SIZE >> 12) << kstream::newline;

	unsigned int current_address = 0x1000U;

	// Set up the code section:
	for(unsigned int i = 0; i < app_pages; ++i, current_address += 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RO, mmu::CODE, false);
	}
	// Set up the data section:
	for(unsigned int i = 0; i < data_pages; ++i, current_address += 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RW, mmu::DATA, false);
	}

	// Set the value of the initial program break:
	program_break = (void *) current_address;

	// Set up the stack:
	current_address = 0x80000000U - 4096U;
	for(unsigned int i = 0; i < stack_pages; ++i, current_address -= 4096)
	{
		void * page = mm::allocate_page();
		translation_table->map_page(page, (void *) current_address, mmu::RW_RW, mmu::STACK, false);
	}

	// Set the application stack pointer:
	registers.r[13] = 0x80000000;

	// Set the application as the current one:
	mmu::set_application_table(translation_table, 1);

	// Copy the application data into the 
	for(unsigned int i = 0; i < app_header->data_start - 0x1000U; ++i)
		*((char *) 0x1000 + i) = buffer[i];
	for(unsigned int i = 0; i < app_header->bss_start - app_header->data_start; ++i)
		*((char *) 0x1000 + (app_pages * 0x1000) + i) = buffer[i + (app_pages * 0x1000)];

	delete[] buffer;
}

process::~process()
{
	delete translation_table;
}

