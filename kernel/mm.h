// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_MM_H
#define WAGTAIL_MM_H

#include "io.h"
#include "kstack.h"
#include "static_kstack.h"
#include "kstream.h"
#include "mmu.h"
#include "hardware/sdrc.h"
#include "utils.h"

namespace wagtail
{
	// Initializes the memory manager, called from assembler code after the
	// MMU has been enabled:
	extern "C" void mm_init();

	class mm final
	{
		public:
			// Initializes the memory and page manager:
			static void initialize();

			// Prints some debug information about the current memory manager state:
			static void print_debug_info(kostream & output);

			// Allocates a chunk of memory with the specified alignment:
			static void * allocate(unsigned int size, unsigned int alignment = 4);
			// Frees a previously allocated chunk of memory:
			static void free(void * memory);

			// Returns a pointer to an unused page:
			static void * allocate_page();
			// Frees a previously allocated page:
			static void free_page(void * pg);

			// Gets the size of the RAM:
			static unsigned int get_ramsize() { return ramsize; }
		private:
			class block
			{
				public:
					block(block * prev, block * next, unsigned int size, bool used = true);

					// Zeroes the memory space of the block:
					void zero();

					// Returns true if the block is in use:
					bool is_used() const { return used; }
					// Sets the current status of the block:
					void set_used(bool used = true) { this->used = used; }
					// Returns the size of the block, in bytes:
					unsigned int get_size() const { return size; }
					// Sets the size of the block:
					void set_size(unsigned int new_size) { size = new_size; }
					// Returns the address of the available memory in the block:
					void * get_address() const { return (void *) ((unsigned int) this + sizeof(block)); }

					// Splits the specified block at the specified offset into the block:
					block * split(unsigned int offset);

					// Merges two contigouous blocks of memory:
					static block * merge(block * a, block * b);

					// Checks if two blocks of memory are contigouous:
					static bool is_contigouous(const block * a, const block * b);
				private:
					block * prev, * next;
					unsigned int size;
					bool used;

					friend class mm;
			};

			// First memory block:
			static block * first_block;
			// The stack of page addresses:
			static kstack<void *> * page_stack;

			// The size of RAM in bytes:
			static unsigned int ramsize;

			// The physical base address of the SDRC:
			static constexpr void * const sdrc_physical = reinterpret_cast<void *>(0x6d000000);

			// The virtual base address of the SDRC:
			static void * sdrc_base;

			// End of allocated kernel data:
			static void * kernel_data_end;
			// End of the mapped kernel data space:
			static void * kernel_dataspace_end;

			// Set to true when the memory manager has been initialized:
			static bool initialization_completed;
			// Set to true when the page stack has been filled and initialized:
			static bool page_stack_initialized;

			// The MMU initialization requires access to the above variables:
			friend class mmu;
	};
}

// Operator for allocating aligned memory:
void * operator new(unsigned int size, unsigned int alignment);
// Operator for allocating memory with default alignment:
void * operator new(unsigned int size);

// Operator for allocating arrays:
void * operator new[](unsigned int size);
// Operator for allocating aligned arrays:
void * operator new[](unsigned int size, unsigned int alignment);

// Operator for freeing allocated memory blocks:
void operator delete(void * data);
// Operator for freeing allocated arrays:
void operator delete[](void * data);

#endif

