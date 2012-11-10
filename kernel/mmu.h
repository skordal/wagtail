// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_MMU_H
#define WAGTAIL_MMU_H

#include "mm.h"

namespace wagtail
{
	// Function called from assembler code to initialize the MMU:
	extern "C" void mmu_init();

	class mmu final
	{
		public:
			// Memory interval access permissions, Kernel_User:
			typedef enum {
				RW_RW,
				RW_RO,
				RW_NA,
				RO_RO,
				RO_NA,
				NA_NA
			} permissions_t;

			// Memory interval type:
			typedef enum {
				CODE,
				DATA,
				RODATA,
				STACK,
				DEVICE
			} interval_type_t;

			// Translation table class, which contains space for the specified
			// amount of entries:
			template<int entries> class __attribute((packed)) translation_table
			{
				public:
					// Take care not to add a constructor here! The kernel
					// translation table is static and used before global
					// constructors are run, so if this class were to require
					// a constructor, it would not be run until after the
					// class has been used!

					// Maps an interval in the page table:
					void map_interval(void * start, void * end, void * virtual_address,
						permissions_t permissions, interval_type_t type);
					// Unmaps an interval of physical memory:
					void unmap_interval(void * start, void * end);

					// Maps a page:
					void map_page(void * page, void * virt, permissions_t permissions,
						interval_type_t type);
					// Unmaps a page:
					void unmap_page(void * page);
				private:
					unsigned int table[entries] __attribute((aligned(4*entries)));
			};

			// Kernel translation table type:
			typedef translation_table<4096> kernel_translation_table_t;
			// Application translation table type:
			typedef translation_table<2048> application_translation_table_t;

			// First level page table entry types:
			struct l1
			{
				static const int type_fault = 0b00;
				static const int type_page_table = 0b01;
				static const int type_section = 0b10;
				static const int type_supersection = (1 << 18)|0b10;
			};

			// Second level page table entry types:
			struct l2
			{
				static const int type_fault = 0b00;
				static const int type_large_page = 0b01;
				static const int type_small_page = 0b10;

				// Small page attribute bits:
				struct small_page
				{
					static const int ng =	1 << 11;
					static const int s =	1 << 10;
					static const int ap2 =	1 <<  9;
					static const int tex2 = 1 <<  8;
					static const int tex1 = 1 <<  7;
					static const int tex0 = 1 <<  6;
					static const int ap1 = 	1 <<  5;
					static const int ap0 =	1 <<  4;
					static const int c =	1 <<  3;
					static const int b = 	1 <<  2;
					static const int xn = 	1 <<  0;
				};
			};

			// Creates the initial page tables and initializes the MMU:
			static void initialize();

			// Clears the TLB:
			static inline void clear_tlb() { asm volatile("mcr p15, 0, r0, c8, c7, 0\n\t"); }

			// Maps a device into the kernel translation table,
			// starting at the specified base and continuing up until
			// base + size (rounded up to align with 4096 bytes).
			// The function returns the address of the start address
			// of the mapping.
			static void * map_device(void * base, unsigned int size);

			// Gets a reference to the kernel translation table:
			static kernel_translation_table_t & get_kernel_table() { return kernel_translation_table; }
		private:
			// Page table class:
			class __attribute((packed)) page_table
			{
				public:
					// Constructs and zeroes the page table:
					page_table();

					// Adds an entry to the page table:
					void add_entry(int offset, void * physical, permissions_t permissions,
						interval_type_t type);
					// Removes an entry from the page table:
					void remove_entry(int offset);
				private:
					unsigned int table[256];
			};

			// Address that the next mapped device is mapped to:
			static void * next_device_addr;

			// The kernel memory translation table:
			static kernel_translation_table_t kernel_translation_table;
	};
}

#endif

