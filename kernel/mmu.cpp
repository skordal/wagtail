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
mmu::kernel_translation_table_t __attribute((aligned(16*1024))) mmu::kernel_translation_table;

// The next ASID:
char mmu::asid = 0;

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

void mmu::set_application_table(application_translation_table_t * table, unsigned int pid)
{
	void * table_physical = virtual_to_physical(table);

	asm volatile(
		// Set the TTBCR.PD0 to 1:
		"mrc p15, 0, ip, c2, c0, 2\n\t"
		"orr ip, #(1 << 4)\n\t"
		"mrc p15, 0, ip, c2, c0, 2\n\t"
		// Change the CONTEXTIDR:
		"mov ip, %[pid], lsl #8\n\t"
		"orr ip, %[asid]\n\t"
		"mcr p15, 0, ip, c13, c0, 1\n\t"
		// Change the TTBR0 register:
		"orr ip, %[descriptor_table], #(0b10 << 3)|0b11\n\t"
		"mcr p15, 0, ip, c2, c0, 0\n\t"
		"isb\n\t"
		// Change the TTBCR.PD0 back to 0:
		"mrc p15, 0, ip, c2, c0, 2\n\t"
		"mvn v1, #(1 << 4)\n\t"
		"and ip, ip, v1\n\t"
		"mcr p15, 0, ip, c2, c0, 2\n\t"
		:
		: [asid] "r" (asid), [pid] "r" (pid), [descriptor_table] "r" (table_physical)
		: "v1", "ip"
	);

	++asid;
}

void * mmu::virtual_to_physical(void * virt)
{
	unsigned int retval, offset = (unsigned int) virt & 0xfff;

	// The resolution of the MMU translation instruction seems to be 4096 bytes, so
	// clear the unused bits:
	virt = (void *) ((unsigned int) virt & 0xfffff000);

	asm volatile(
		"mcr p15, 0, %[virt], c7, c8, 0\n\t"
		"isb\n\t"
		"mrc p15, 0, %[retval], c7, c4, 0\n\t"
		: [retval] "=r" (retval)
		: [virt] "r" (virt)
		:
	);

	return (void *) ((retval & 0xfffff000) + offset);
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
void mmu::page_table::add_entry(int offset, void * physical, permissions_t permissions, interval_type_t type, bool not_global)
{
	unsigned int entry = reinterpret_cast<unsigned int>(physical) | mmu::l2::type_small_page;

	if(not_global)
		entry |= l2::small_page::ng;

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
void * mmu::page_table::remove_entry(int offset)
{
	void * retval = reinterpret_cast<void *>(table[offset] & 0xfffff000);
	table[offset] = 0;
	return retval;
}

// Checks if a page table is empty:
bool mmu::page_table::is_empty() const
{
	for(int i = 0; i < 256; ++i)
		if(table[i] != 0)
			return false;
	return true;
}

