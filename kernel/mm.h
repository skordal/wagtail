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
	/**
	 * Initializes the kernel memory manager. This method is called from assembly code
	 * after the MMU has been enabled, and is simply a wrapper for `mm::initialize()`.
	 */
	extern "C" void mm_init();

	class mm final
	{
		public:
			/** Amount of memory to reserve for the kernel heap. */
			static const unsigned int KERNEL_HEAP_SIZE = 4 * 1024 * 1024;

			/** Initializes the kernel memory manager. */
			static void initialize();

			/** Prints some statistics about the current state of the memory manager. */
			static void print_debug_info(kostream & output);

			/**
			 * Allocates a block of memory.
			 * @param size the size of the block to allocate.
			 * @param alignment the alignment of the block of memory. Note that alignments
			 *                  less than 4 will be rounded up.
			 * @return a newly allocated block of memory or `nullptr` if there is no more memory
			 *         available.
			 */
			static void * allocate(unsigned int size, unsigned int alignment = 4);
			/**
			 * Frees a previously allocated block of memory.
			 * @param memory the memory block to free.
			 */
			static void free(void * memory);

			/**
			 * Allocates a page.
			 * @return an unused page or `nullptr` if the system is out of memory.
			 */
			static void * allocate_page();
			/**
			 * Frees a previously allocated page.
			 * @param pg the page to free.
			 */
			static void free_page(void * pg);

			/**
			 * Gets the size of RAM.
			 * @return the size of RAM in bytes.
			 */
			static unsigned int get_ramsize() { return ramsize; }
		private:
			/**
			 * Class representing a memory block.
			 */
			class block
			{
				public:
					/**
					 * Constructs a new block of memory.
					 * @param prev a pointer to the previous memory block header.
					 * @param next a pointer to the next memory block header.
					 * @param size the size of the block.
					 * @param used `true` if the block has been allocated, `false` otherwise.
					 */
					block(block * prev, block * next, unsigned int size, bool used = true);

					/** Zeroes the contents of the memory block. */
					void zero();

					/**
					 * Returns whether the block has been allocated.
					 * @return `true` if the block is in use, `false` otherwise.
					 */
					bool is_used() const { return used; }
					/**
					 * Sets the current status of the block.
					 * @param used `true` if the block has been allocated, `false` otherwise.
					 */
					void set_used(bool used = true) { this->used = used; }

					/**
					 * Gets the size of the block.
					 * @return the size of the block in bytes.
					 */
					unsigned int get_size() const { return size; }
					/**
					 * Sets the size of the block.
					 * @param new_size the new size of the block.
					 */
					void set_size(unsigned int new_size) { size = new_size; }

					/**
					 * Returns the address of the memory for this block.
					 * @return the address of the memory for this block.
					 */
					void * get_address() const { return (void *) ((unsigned int) this + sizeof(block)); }

					/**
					 * Splits the block at the specified offset into the block.
					 * @param offset offset to split the block at.
					 * @return the block that was split off.
					 */
					block * split(unsigned int offset);

					/**
					 * Merges two contigouous blocks of memory into one.
					 * @param a the first block.
					 * @param b the second block.
					 * @return the merged block.
					 */
					static block * merge(block * a, block * b);

					/**
					 * Checks if two blocks of memory are contigouous.
					 * @param a the first block.
					 * @param b the second block.
					 * @return `true` if the blocks are contigouous, `false` otherwise.
					 */
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

