// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_MMU_H
#define WAGTAIL_MMU_H

#include "kstream.h"
#include "mm.h"
#include "rbtree.h"

namespace wagtail
{
	/**
	 * Initializes the kernel translation tables before enabling the MMU.
	 * This method is called from assembly code in `start.S`, and is a
	 * wrapper for `mmu::initialize()`.
	 */
	extern "C" void mmu_init();

	/**
	 * Class representing the MMU.
	 *
	 * This class handles the setup of translation tables and page tables.
	 */
	class mmu final
	{
		public:
			/** Memory access permissions. */
			typedef enum {
				RW_RW, /**< Kernel: read-write; user: read-write. */
				RW_RO, /**< Kernel: read-write; user: read-only. */
				RW_NA, /**< Kernel: read-write; user: no access. */
				RO_RO, /**< Kernel: read-only; user: read-only. */
				RO_NA, /**< Kernel: read-only; user: no access. */
				NA_NA  /**< No access for anyone. */
			} permissions_t;

			/**
			 * Memory interval types.
			 * This is used to make sure different sections of memory get
			 * correct attributes.
			 */
			typedef enum {
				CODE,   /**< Section containing executable code. */
				DATA,   /**< Section containing read-write data. */
				RODATA, /**< Section containing read-only data. */
				STACK,  /**< Section containing a stack. */
				DEVICE  /**< Section containing device memory. */
			} interval_type_t;

			/**
			 * Class representing a translation table.
			 * @tparam entries the number of entries in the translation table.
			 */
			template<int entries> class translation_table
			{
				public:
					translation_table() {}

					/** Clears the translation table by zeroing all entries. */
					void clear()
					{
						for(int i = 0; i < entries; ++i)
							table[i] = 0;
					}

					/**
					 * Maps an interval of memory in the translation table.
					 * @param start physical start address of the interval.
					 * @param end physical end address of the interval.
					 * @param virtual_address the virtual address to start the mapping at.
					 * @param permissions interval access permissions.
					 * @param type memory interval type.
					 * @param not_global specifies if the mapped memory is global or local to
					 *                   the current translation table.
					 */
					void map_interval(void * start, void * end, void * virtual_address,
						permissions_t permissions, interval_type_t type, bool not_global = false)
					{
						unsigned int current_address = (unsigned int) start;
						unsigned int current_virtual = (unsigned int) virtual_address;

						for(; current_address < (unsigned int) end;
							current_address += 4096, current_virtual += 4096)
						{
							map_page(reinterpret_cast<void *>(current_address),
								reinterpret_cast<void *>(current_virtual),
								permissions, type, not_global);
						}
					}

					/**
					 * Unmaps an interval of virtual memory.
					 * @param start virtual start address of the interval.
					 * @param end virtual end address of the interval.
					 */
					void unmap_interval(void * start, void * end)
					{
						unsigned int current_address = reinterpret_cast<unsigned int>(start);

						for(; current_address < (unsigned int) end; current_address += 4096)
							unmap_page((void *) current_address);
					}

					/**
					 * Maps a physical page to a virtual address.
					 * @param page the physical address of the page.
					 * @param virt the virtual address to map the page to.
					 * @param permissions access permissions for the page.
					 * @param type type of memory for the mapped memory.
					 * @param not_global specified whether the mapped memory is global or
					 *                   not.
					 */
					void map_page(void * page, void * virt, permissions_t permissions,
						interval_type_t type, bool not_global = false)
					{
						page_table * pt;
						unsigned int table_index = (unsigned int) virt >> 20;

						if(table[table_index] & l1::type_page_table)
						{
							void * pt_physical, * pt_virtual = nullptr;
							pt_physical = reinterpret_cast<void *>(table[table_index] & 0xfffffc00);
							if(!directory.get_value(pt_physical, pt_virtual))
								pt = (page_table *) pt_physical;
							else
								pt = (page_table *) pt_virtual;
						} else  { // If no page table exists, allocate a new one and make a page table entry:
							pt = new (1024) page_table;
							directory.insert(virtual_to_physical(pt), pt);
							table[table_index] = (unsigned int) virtual_to_physical(pt) | mmu::l1::type_page_table;
						}

						unsigned int pt_index = ((unsigned int) virt & 0x000fffff) >> 12;
						pt->add_entry(pt_index, page, permissions, type, not_global);
					}

					/**
					 * Unmaps a page from a virtual address.
					 * @param page the virtual address of the page.
					 */
					void unmap_page(void * page)
					{
						unsigned int address = reinterpret_cast<unsigned int>(page);
						void * pt_physical, * pt_virtual;
						page_table * pt;

						if(!(table[address >> 20] & l1::type_page_table))
							return;

						pt_physical = (void *) (table[address >> 20] & 0xfffffc00);
						if(!directory.get_value(pt_physical, pt_virtual))
							pt = reinterpret_cast<page_table *>(pt_physical);
						else
							pt = reinterpret_cast<page_table *>(pt_virtual);

						void * freed_page = pt->remove_entry((address & 0x000fffff) >> 12);
						if(freed_page != nullptr)
							mm::page_stack->push(freed_page);

						if(pt->is_empty())
						{
							table[address >> 20] = 0;
							delete pt;
						}
					}
				private:
					unsigned int table[entries] __attribute((aligned(4*entries)));
					// Directory for doing physical->virtual translations for page tables:
					rbtree<void *, void *> directory;
			};

			/** Kernel translation table type (4096 entries). */
			typedef translation_table<4096> kernel_translation_table_t;
			/** Application translation table type (2048 entries). */
			typedef translation_table<2048> application_translation_table_t;

			/** Constants used in first level translation tables. */
			struct l1
			{
				static const int type_fault = 0b00;
				static const int type_page_table = 0b01;
				static const int type_section = 0b10;
				static const int type_supersection = (1 << 18)|0b10;
			};

			/** Constants used in second level translation tables. */
			struct l2
			{
				static const int type_fault = 0b00;
				static const int type_large_page = 0b01;
				static const int type_small_page = 0b10;

				/** Small page (4096 bytes) attribute bits. */
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

			/**
			 * Initializes the MMU.
			 * Sets up the initial kernel translation table and some MMU registers. The
			 * MMU is not actually enabled until after this method has been executed.
			 */
			static void initialize();

			/** Clears the unified TLB. */
			static void clear_tlb() { asm volatile("mcr p15, 0, r0, c8, c7, 0\n\t"); }

			/**
			 * Maps a device into the kernel translation table.
			 * @param base the physical base address of the device's address space.
			 * @param size the size of the device's address space.
			 * @return the virtual address of the device.
			 */
			static void * map_device(void * base, unsigned int size);

			/**
			 * Gets a reference to the kernel translation table.
			 * @return a reference to the kernel translation table.
			 */
			static kernel_translation_table_t & get_kernel_table() { return kernel_translation_table; }

			/**
			 * Sets the current application translation table.
			 * @param table the new application translation table to use.
			 * @param pid the process ID of the process to switch to.
			 */
			static void set_application_table(application_translation_table_t * table, unsigned int pid);

			/**
			 * Converts the specified virtual address into a physical address.
			 * This is done using MMU registers.
			 * @param virt the virtual address to translate.
			 * @return the physical address corresponding to the virtual.
			 */
			static void * virtual_to_physical(void * virt);
		private:
			/**
			 * Class representing a page table.
			 */
			class page_table
			{
				public:
					/** Constructs and zeroes a page table object. */
					page_table();

					/**
					 * Adds an entry to the translation table.
					 * @param offset offset into the page table.
					 * @param the physical address to use in the mapping.
					 * @param permissions permissions for the mapping.
					 * @param type memory interval type for the mapping.
					 */
					void add_entry(int offset, void * physical, permissions_t permissions,
						interval_type_t type, bool not_global = false);
					/**
					 * Removes a mapping from the table.
					 * @param offset the offset into the table to remove the mapping from.
					 * @return the physical address of the page mapped to the specified entry.
					 */
					void * remove_entry(int offset);

					/**
					 * Checks if a page table is empty.
					 * @return `true` if the table is empty, `false` otherwise.
					 */
					bool is_empty() const;
				private:
					unsigned int table[256];
			};
			static_assert(sizeof(page_table) == 1024, "mmu::page_table must be 1024 bytes long!");

			// Address that the next mapped device is mapped to:
			static void * next_device_addr;

			// The kernel memory translation table:
			static kernel_translation_table_t kernel_translation_table;

			// The ASID:
			static char asid;
	};
}

#endif

