// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "mmu.h"

using namespace wagtail;

// Symbols from the linker script:
extern void * text_start;
extern void * data_start, * data_end;

// Address of the next device mapping:
void * mmu::next_device_addr = reinterpret_cast<void *>(0xc0000000);

// Kernel page table:
mmu::kernel_translation_table_t mmu::kernel_translation_table;

// Initializes the MMU by setting up the kernel page table:
extern "C" void mmu_init()
{
	mmu::initialize();
}

// Creates the initial page tables and initializes the MMU:
void mmu::initialize()
{
	// TODO: Unhardcode these values:
	void * stack_start_physical = reinterpret_cast<void *>(0x80000000);
	void * stack_end_physical = reinterpret_cast<void *>(0x80008000);
	void * stack_virtual = reinterpret_cast<void *>(0xa0000000 - 0x8000);

	// Map the kernel code:
	kernel_translation_table.map_interval(&text_start, &data_start, &text_start, RO_NA, CODE);
	// Map the kernel data:
	kernel_translation_table.map_interval(&data_start, &data_end, &data_start, RW_NA, DATA);
	// Map the kernel stack:
	kernel_translation_table.map_interval(stack_start_physical, stack_end_physical, stack_virtual,
		RW_NA, STACK);

	// Map the UART for testing purposes:
	//kernel_translation_table.map_page((void *) 0x49020000, (void *) 0x49020000, RW_RW, DEVICE);

	// Map the dataspace that may have been allocated for page tables:
	while(reinterpret_cast<unsigned int>(mm::kernel_data_end) >= reinterpret_cast<unsigned int>(mm::kernel_dataspace_end))
	{
		kernel_translation_table.map_page(mm::kernel_dataspace_end, mm::kernel_dataspace_end, RW_NA, DATA);
		mm::kernel_dataspace_end =
			reinterpret_cast<void *>(reinterpret_cast<unsigned int>(mm::kernel_dataspace_end) + 4096);
	}

	// Set registers and parameters:
	clear_tlb();
	unsigned int translation_table_address = (unsigned int) &kernel_translation_table;
	asm volatile(
		// Set the TTBCR register to N = 1 (causes 8096 byte application translation tables):
		"movs v1, #1\n\t"
		"mcr p15, 0, v1, c2, c0, 2\n\t"
		// Set TTBR1 to the kernel descriptor table:
		"orr v1, %[kernel_descriptor_table], #(0b10 << 3)|0b11\n\t"
		"mcr p15, 0, v1, c2, c0, 1\n\t"
		// Set domain permissions:
		"ldr v1, =0x55555555\n\t"
		"mcr p15, 0, v1, c3, c0, 0\n\t"
		:
		: [kernel_descriptor_table] "r" (translation_table_address)
		: "v1", "cc"
	);
	clear_tlb();
}

// Maps a device into the kernel translation table and returns the address that
// the device was mapped to:
void * mmu::map_device(void * base, unsigned int size)
{
	void * retval = next_device_addr;

	kernel_translation_table.map_interval(
		base, reinterpret_cast<void *>(reinterpret_cast<unsigned int>(base) + size),
		next_device_addr, RW_NA, DEVICE);
	next_device_addr = reinterpret_cast<void *>(reinterpret_cast<unsigned int>(next_device_addr) + size);
	return retval;
}

// Constructs and zeroes a page table:
mmu::page_table::page_table()
{
	// This constructor merely exists to prevent GCC from attempting to
	// run memset to zero the memory of the class. As long as the constructor
	// exists, GCC will run the constructor instead after a page_table object
	// has been allocated.

	for(int i = 0; i < 256; ++i)
		table[i] = 0;
}

// Adds an entry to the page table:
void mmu::page_table::add_entry(int offset, void * physical, permissions_t permissions, interval_type_t type)
{
	unsigned int entry = reinterpret_cast<unsigned int>(physical) | mmu::l2::type_small_page;

	switch(type)
	{
		case CODE:
			entry |=
				l2::small_page::tex2 |
				l2::small_page::tex1 |
				l2::small_page::c |
				l2::small_page::s;
			break;
		case DATA:
			entry |= l2::small_page::xn |
				l2::small_page::tex2 |
				l2::small_page::tex0 |
				l2::small_page::b |
				l2::small_page::s;
			break;
		case RODATA:
			entry |= l2::small_page::xn |
				l2::small_page::tex2 |
				l2::small_page::tex1 |
				l2::small_page::tex0 |
				l2::small_page::c |
				l2::small_page::b |
				l2::small_page::s;
			break;
		case STACK:
			entry |= l2::small_page::xn |
				l2::small_page::tex2 |
				l2::small_page::tex0 |
				l2::small_page::b |
				l2::small_page::s;
			break;
		case DEVICE:
			entry |= l2::small_page::xn |
				l2::small_page::b |
				l2::small_page::s;
			break;
	}

	switch(permissions)
	{
		case RW_RW:
			entry |= l2::small_page::ap1 | l2::small_page::ap0;
			break;
		case RW_RO:
			entry |= l2::small_page::ap1;
			break;
		case RW_NA:
			entry |= l2::small_page::ap0;
			break;
		case RO_RO:
			entry |= l2::small_page::ap2 | l2::small_page::ap1 | l2::small_page::ap0;
			break;
		case RO_NA:
			entry |= l2::small_page::ap2 | l2::small_page::ap0;
			break;
		case NA_NA:
			break;
	}

	table[offset] = entry;
}

// Removes an entry from the page table:
void mmu::page_table::remove_entry(int offset)
{
	table[offset] = 0;
}

// Checks if a page table is empty:
bool mmu::page_table::is_empty() const
{
	for(int i = 0; i < 256; ++i)
		if(table[i] != 0)
			return false;
	return true;
}

