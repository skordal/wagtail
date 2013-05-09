// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "mm.h"

using namespace wagtail;
using namespace wagtail::hardware;

// Symbols imported from the linker script:
extern void * bss_end, * data_end;
extern void * ramstart;

// Location of the first block of allocated memory:
mm::block * mm::first_block = nullptr;
// Stack of page addresses:
kstack<void *> * mm::page_stack = nullptr;

// Virtual base address of the SDRC module:
void * mm::sdrc_base = nullptr;

// The ramsize stores the size of RAM:
unsigned int mm::ramsize = 0;

// The end of the allocated kernel memory, used during initialization:
void * mm::kernel_data_end = &bss_end;
// The end of the mapped kernel data space:
void * mm::kernel_dataspace_end = &data_end;

// Set to true when the memory manager has been initialized:
bool mm::initialization_completed = false;
// Set to true when the page stack has been filled and initialized:
bool mm::page_stack_initialized = false;

// Memory management initialization function, called from assembler code after the
// MMU has been enabled:
extern "C" void mm_init()
{
	mm::initialize();
}

// Initializes the memory manager, done after the MMU has been initialized, but
// before it has been enabled.
void mm::initialize()
{
	// Map the SDRC memory space:
	sdrc_base = mmu::map_device(sdrc_physical, 64 * 1024);

	// Get the amount of available RAM:
	ramsize += (io::read<unsigned int>(sdrc_base, sdrc::registers::mcfg0::offset) >> 8) & 0x3ff;
	ramsize += (io::read<unsigned int>(sdrc_base, sdrc::registers::mcfg1::offset) >> 8) & 0x3ff;
	// The amount of RAM reported is in units of 2 Mb, so multiply by 2^21 to get bytes:
	ramsize <<= 21;

	// Align the kernel dataspace end to 4096:
	kernel_dataspace_end = (void *) (((unsigned int) kernel_dataspace_end + 4095) & utils::mask_left(4096));

	// Map the kernel heap:
	const unsigned int mapping_start = ((unsigned int) kernel_dataspace_end + 4095) & utils::mask_left(4096);
	const unsigned int mapping_end = (mapping_start + KERNEL_HEAP_SIZE) & utils::mask_left(4096);
	mmu::get_kernel_table().map_interval((void *) mapping_start, (void *) mapping_end, kernel_dataspace_end, mmu::RW_NA, mmu::DATA);
	kernel_dataspace_end = reinterpret_cast<void *>(mapping_end);

	// Create the first block:
	first_block = reinterpret_cast<block *>(kernel_data_end);
	first_block->next = 0;
	first_block->prev = 0;
	first_block->used = false;
	first_block->size = (unsigned int) kernel_dataspace_end - (unsigned int ) kernel_data_end - sizeof(block);
	first_block->zero();

	// Allow use of the memory allocator (TODO: Rename this variable):
	initialization_completed = true;

	// Create a stack of available pages for the page allocator:

	// Figure out how many pages are available currently:
	unsigned int num_pages = (ramsize / 4096) -
		(((unsigned int) kernel_dataspace_end - (unsigned int) &ramstart) / 4096);
	page_stack = new static_kstack<void *>(num_pages);

	for(unsigned int page_addr = (unsigned int) kernel_dataspace_end;
		page_addr < ((unsigned int) &ramstart + ramsize); page_addr += 4096)
		page_stack->push((void *) page_addr);

	page_stack_initialized = true;
}

// Prints some debug statistics about the current memory manager state:
void mm::print_debug_info(kostream & out)
{
	out << "Total RAM size: " << static_cast<int>(ramsize) << " bytes" << kstream::newline;
	out << "Block header size: " << (int) sizeof(block) << " bytes" << kstream::newline;
	out << "Kernel dataspace end: " << kernel_dataspace_end << kstream::newline;
	out << "Block info: " << kstream::newline;

	block * current_block = first_block;
	int block_counter = 0;

	do {
		out << "\tBlock " << block_counter++ << " @ " << (void *) current_block
			<< ": " << (int) current_block->get_size() << " bytes - "
			<< (current_block->is_used() ? "used" : "unused") << kstream::newline;
	} while((current_block = current_block->next) != 0);
}

// Allocates a chunk of aligned memory:
void * mm::allocate(unsigned int size, unsigned int alignment)
{
	unsigned int return_address = 0;

	// Ignore requests for 0 bytes of memory:
	if(size == 0)
		return nullptr;

	// Round the size up to a multiple of 4 to maintain alignment
	// and prevent data aborts:
	size += 3;
	size &= 0xfffffffc;

	// As this function may also used for allocating memory during initialization of the MMU,
	// a different method of allocating memory is supported; if the MMU is disabled, a pointer to the
	// memory after the end of the kernel is returned and then incremented.
	if(!initialization_completed)
	{
		return_address = (unsigned int) mm::kernel_data_end;

		// It is unlikely that it will be neccessary to change the alignment
		// of the return address; however, future changes may make it neccessary,
		// so this code is here just in case:
		if((return_address & ~utils::mask_left(alignment)) != 0)
			return_address = (return_address + (alignment - 1)) & utils::mask_left(alignment);
		mm::kernel_data_end = reinterpret_cast<void *>(return_address + size);

		// Zero the allocated memory:
		for(unsigned int i = 0; i < size; ++i)
			*reinterpret_cast<char *>(return_address + i) = 0;
	} else { // If the MMU is enabled, a block of memory is allocated.
		block * current_block = first_block;

		do {
			// Check if the block has enough free space:
			if(current_block->get_size() >= size && !current_block->is_used())
			{
				return_address = reinterpret_cast<unsigned int>(current_block->get_address());

				// Get the offset into the block needed for correct alignment:
				unsigned int offset = ((return_address + alignment - 1) & utils::mask_left(alignment))
					- return_address;

				if(offset == 0)
				{
					if(current_block->get_size() > size)
						current_block->split(size);
					current_block->set_used();
					return_address = reinterpret_cast<unsigned int>(current_block->get_address());
					break;
				} else if(offset > sizeof(block) && current_block->get_size() - offset >= size)
				{
					block * split_block = current_block->split(offset - sizeof(block));
					if(split_block->get_size() > size)
						split_block->split(size);
					split_block->set_used();
					return_address = reinterpret_cast<unsigned int>(split_block->get_address());
					break;
				}
			}

			current_block = current_block->next;
		} while(current_block != nullptr);
	}

	return reinterpret_cast<void *>(return_address);
}

// Frees a block of allocated memory:
void mm::free(void * memory_block)
{
	if(memory_block == nullptr)
		return;

	block * current = reinterpret_cast<block *>((unsigned int) memory_block - sizeof(block));
	current->set_used(false);

	if(current->prev != nullptr && !current->prev->is_used())
		current = mm::block::merge(current->prev, current);
	if(current->next != nullptr && !current->next->is_used())
		current = mm::block::merge(current, current->next);
}

// Allocates a page:
void * mm::allocate_page()
{
	// When initializing the memory allocation function may need to allocate
	// new pages. When that happens, simply return the space after the currently
	// allocated kernel dataspace. These pages will not be freed later.
	if(!page_stack_initialized)
		return kernel_dataspace_end;
	else {
		if(!page_stack->is_empty())
			return page_stack->pop();
		else
			return nullptr;
	}
}

// Frees an allocated page:
void mm::free_page(void * page)
{
	page_stack->push(page);
}

// Block constructor:
mm::block::block(block * prev, block * next, unsigned int size, bool used)
	: prev(prev), next(next), size(size), used(used)
{

}

// Splits a block of memory, or returns null if this is not possible. Returns the
// block that has been split off the current block if successful. The offset is the
// offset from the start of the block's memory area.
// | ---- A ---- | => |-- A --||-- B --|
mm::block * mm::block::split(unsigned int offset)
{
	// Make sure there is room to split the block:
	if(size - offset <= sizeof(block))
		return 0;

	block * new_block = reinterpret_cast<block *>((unsigned int) get_address() + offset);

	new_block->next = next;
	new_block->prev = this;
	next = new_block;
	if(new_block->next != 0)
		new_block->next->prev = new_block;

	new_block->used = false;
	new_block->size = size - (offset + sizeof(block));

	size -= new_block->size + sizeof(block);

	return new_block;
}

// Zeroes the block of memory:
void mm::block::zero()
{
	unsigned char * block_space = reinterpret_cast<unsigned char *>(this) + sizeof(block);
	for(unsigned int i = 0; i < size; ++i)
		block_space[i] = 0;
}

// Merges two contigouous blocks of memory. Returns the resulting block.
// |--- A ---||--- B ---| => |--- A ---  --- B ---|
mm::block * mm::block::merge(block * a, block * b)
{
	if(!is_contigouous(a, b))
		return 0;

	a->size += b->get_size() + sizeof(block);
	a->next = b->next;

	if(b->next != 0)
		b->next->prev = a;

	return a;
}

// Checks if two blocks are contigouous, and that A is in front of B:
bool mm::block::is_contigouous(const block * a, const block * b)
{
	if(a->next == b && b->prev == a)
		return true;
	else
		return false;
}

// Operator for allocating aligned memory:
void * operator new(unsigned int size, unsigned int alignment)
{
	return mm::allocate(size, alignment);
}

// Operator for allocating memory with the default alignment:
void * operator new(unsigned int size)
{
	return mm::allocate(size);
}

// Operator for allocating arrays:
void * operator new[](unsigned int size)
{
	return mm::allocate(size);
}

// Operator for allocating aligned memory arrays:
void * operator new[](unsigned int size, unsigned int alignment)
{
	return mm::allocate(size, alignment);
}

// Operator for freeing memory:
void operator delete(void * block)
{
	return mm::free(block);
}

// Operator for deleting arrays:
void operator delete[](void * block)
{
	return mm::free(block);
}

