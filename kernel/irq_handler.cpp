// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "irq_handler.h"

using namespace wagtail;
using namespace wagtail::hardware::intc;

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

	for(int i = 0; i < NUMBER_OF_IRQS; ++i)
		irq_handlers[i] = nullptr;

	int revision = io::read<int>(virtual_base, registers::revision::offset);
	kernel::message() << "\tInterrupt controller version: "
		<< (revision >> 4) << "." << (revision & 0x0f) << kstream::newline;

	io::write<int>(DEFAULT_THRESHOLD, virtual_base, registers::threshold::offset);
	io::write<int>(registers::protection::enable, virtual_base, registers::protection::offset);

	// Allocate an IRQ stack (4 Kb below the kernel stack):
	void * irq_stack_virtual = reinterpret_cast<void *>(0xa0000000 - 0x8000 - 0x1000);
	irq_stack = mm::allocate_page();
	mmu::get_kernel_table().map_page(irq_stack, irq_stack_virtual, mmu::RW_NA, mmu::STACK);
	kernel::message() << "\tIRQ stack: 4 Kb @ " << irq_stack << kstream::newline;

	// Set up an IRQ stack right below the kernel stack:
	void * stack_address = reinterpret_cast<void *>(0xa0000000 - 0x8000);
	asm volatile(
		"cps #0b10010\n\t" // Go to IRQ mode.
		"mov sp, %[stack_address]\n\t" // Set the new stack address.
		"cps #0b10011\n\t" // Go back to SVC mode.
		:
		: [stack_address] "r" (stack_address)
		: "cc"
	);
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
	io::write<int>(1 << bit, virtual_base, registers::registers::mir_clear_base::offset + (0x20 * mir_num));
}

void irq_handler::unregister_handler(int number)
{
	int mir_num = number >> 5;
	int bit = number & 0x1f;

	irq_handlers[number] = nullptr;
	io::write<int>(1 << bit, virtual_base, registers::registers::mir_set_base::offset + (0x20 * mir_num));
}

void irq_handler::handle_irq(int number)
{
	if(irq_handlers[number] != nullptr) // Prevents bad_function_call exception
		irq_handlers[number](number);
	else
		kernel::message() << "Warning: no IRQ handler for IRQ " << number << "!" << kstream::newline;
}

