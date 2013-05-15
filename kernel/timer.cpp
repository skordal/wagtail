// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "timer.h"
#include "kernel.h"

using namespace wagtail;
using namespace wagtail::hardware::timer;

// Timer module base addresses:
void * timer::timer_base[11] = {
	mmu::map_device(reinterpret_cast<void *>(0x48318000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49032000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49034000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49036000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49038000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x4903a000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x4903c000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x4903e000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49040000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x48086000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x48088000), 4096)
};

timer timer::timer_modules[11] = {
	timer(0),
	timer(1),
	timer(2),
	timer(3),
	timer(4),
	timer(5),
	timer(6),
	timer(7),
	timer(8),
	timer(9),
	timer(10)
};


void timer::start(std::function<void()> callback, timer::mode mode, unsigned int interval)
{
	// The timer runs on a 12 MHz clock by default, so 12 ticks are 1 µs:
	interval *= 12;

	// Set timer parameters:
	this->interval = interval;
	this->callback = callback;

	// Register the IRQ handler for the timer:
	irq_handler::get()->register_handler(&timer::irq_handler, IRQ_BASE + module);

	// Set the timer mode:
	timer_mode = mode;
	switch(mode)
	{
		case timer::mode::disabled:
			kernel::message() << "Error: timer::start called with mode = disabled!" << kstream::newline;
			stop();
			break;
		case timer::mode::one_shot:
			io::and_register(~registers::tclr::ar, timer_base[module], registers::tclr::offset);
			break;
		case timer::mode::repeating:
			io::or_register(registers::tclr::ar, timer_base[module], registers::tclr::offset);
			break;
	}

	// Set the trigger output mode to overflow:
	io::or_register(1 << registers::tclr::trg, timer_base[module], registers::tclr::offset);

	// The interrupt triggers on overflow, so set the timer value to the overflow value
	// minus the interval value:
	io::write(0xffffffffU - interval, timer_base[module], registers::tldr::offset);
	io::write(1, timer_base[module], registers::ttgr::offset);

	// Enable the overflow interrupt:
	io::write(registers::tier::ovf_it_ena, timer_base[module], registers::tier::offset);

	// Finally, start the timer:
	io::or_register(registers::tclr::st, timer_base[module], registers::tclr::offset);
}

void timer::stop()
{
	// Disable the interrupt:
	irq_handler::get()->unregister_handler(IRQ_BASE + module);

	// Stop the timer:
	io::and_register(~registers::tclr::st, timer_base[module], registers::tclr::offset);

	callback = nullptr;
	interval = 0;
}

timer::timer(int n) : module(n)
{
	// Reset the module:
	io::write(registers::tiocp_cfg::softreset, timer_base[n], registers::tiocp_cfg::offset);
	while(!(io::read<int>(timer_base[n], registers::tistat::offset) & registers::tistat::resetdone));

	// Make sure the timer is stopped:
	io::and_register(~registers::tclr::st, timer_base[n], registers::tclr::offset);
}

void timer::irq_handler(int n)
{
	timer & t = timer::get(n - IRQ_BASE);

	// Call the callback function:
	t.callback();

	// Clear the interrupt flag:
	io::and_register(registers::tisr::ovf_it_flag, timer_base[t.module], registers::tisr::offset);

	// If the timer is a one-shot timer, stop it:
	if(t.timer_mode == timer::mode::one_shot)
		t.stop();
	else // Otherwise, reload the interval value:
		io::write(0xffffffffU - t.interval, timer_base[t.module], registers::tldr::offset);
}

