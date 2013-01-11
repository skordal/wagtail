// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "irq_handler.h"

using namespace wagtail;
using namespace wagtail::hardware::intc;

// Virtual base address of the interrupt controller for use by FIQs in interrupts.S:
extern void * intc_base;

// Address of the top of the IRQ stack:
extern void * irq_stack;

irq_handler * irq_handler::global_irq_handler = nullptr;

extern "C" void handle_irq()
{
	int irq_number = io::read<int>(irq_handler::get()->virtual_base, registers::sir_irq::offset) & 0x7f;
	irq_handler::get()->handle_irq(irq_number);

	asm volatile("dsb\n\t");
	io::write<int>(registers::control::newirqagr, irq_handler::get()->virtual_base, registers::control::offset);
}

void irq_handler::initialize()
{
	global_irq_handler = new irq_handler;
	asm volatile("dsb\n\tcpsie if\n\t");
}

irq_handler::irq_handler() : virtual_base(mmu::map_device((void *) 0x48200000, 4096))
{
	kernel::message() << "Initializing IRQ handler:" << kstream::newline;
	intc_base = virtual_base;

	// Allocate an IRQ stack (4Kb at 8 Kb below the kernel stack):
	void * irq_stack_physical = mm::allocate_page();
	void * irq_stack_virtual = reinterpret_cast<void *>(0xa0000000U - 0x8000 - 0x2000);
	mmu::get_kernel_table().map_page(irq_stack_physical, irq_stack_virtual, mmu::RW_NA, mmu::STACK);
	kernel::message() << "\tAllocated IRQ stack: 4 Kb @ " << irq_stack_virtual << kstream::newline;

	// Set up the IRQ stack:
	irq_stack = reinterpret_cast<void *>(0xa0000000U - 0x8000 - 0x1000);

	int revision = io::read<int>(virtual_base, registers::revision::offset);
	kernel::message() << "\tInterrupt controller version: "
		<< (revision >> 4) << "." << (revision & 0x0f) << kstream::newline;

	io::write<int>(DEFAULT_THRESHOLD, virtual_base, registers::threshold::offset);
	io::write<int>(registers::protection::enable, virtual_base, registers::protection::offset);
}

void irq_handler::register_handler(std::function<void(int)> handler, int number, int priority)
{
	int mir_num = number >> 5;
	int bit = number & 0x1f;

	irq_handlers[number] = handler;
	kernel::message() << "IRQ(" << number << "): handler registered" << kstream::newline;

	// Set the interrupt priority:
	io::write<int>((priority << 2) & 0xfc, virtual_base, registers::ilr_base::offset + (number << 2));
	// Enable the interrupt:
	io::write<int>(1 << bit, virtual_base, registers::mir_clear_base::offset + (0x20 * mir_num));
}

void irq_handler::unregister_handler(int number)
{
	int mir_num = number >> 5;
	int bit = number & 0x1f;

	irq_handlers[number] = nullptr;
	io::write<int>(1 << bit, virtual_base, registers::mir_set_base::offset + (0x20 * mir_num));
}

void irq_handler::handle_irq(int number)
{
	irq_handlers[number](number);
}

