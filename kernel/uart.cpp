// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "uart.h"

using namespace wagtail;
using namespace wagtail::hardware::uart;

// UART base addresses:
void * uart::uart_base[4] = {
	mmu::map_device(reinterpret_cast<void *>(0x4906a000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x4806c000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49020000), 4096),
	mmu::map_device(reinterpret_cast<void *>(0x49042000), 4096)
};

// Available UART devices:
uart uart::uart_modules[4] = {uart(0), uart(1), uart(2), uart(3)};

// Writes a character to a UART module:
kostream & uart::operator << (char character)
{
	// For newlines, a \r must also be printed:
	if(character == '\n')
		*this << '\r';

	// Wait for the UART to be ready to send:
	while(io::read<char>(uart_base[module], registers::ssr::offset) & registers::ssr::tx_fifo_full);
	io::write(character, uart_base[module], registers::thr::offset);

	return *this;
}

unsigned int uart::write(const char * buffer, unsigned int size, unsigned int flags)
{
	for(unsigned int i = 0; i < size; ++i)
		*this << buffer[i];
	return size;
}

// Constructs a new UART module with the specified parameters:
uart::uart(int module) : module(module)
{
	if(module != 2) // None of the other UARTs are in use, so we do nothing with them for now.
		return;

	// Reset the controller:
	io::or_register<char>(registers::sysc::softreset, uart_base[module], registers::sysc::offset);
	while(io::read<char>(uart_base[module], registers::sysc::offset) & registers::sysc::softreset);

	// Set power saving bits:
	io::write<char>(registers::sysc::autoidle|registers::sysc::idlemode(registers::sysc::smart),
		uart_base[module], registers::sysc::offset);

	// Enable sleep mode:
	io::or_register(registers::efr::enhanced_en, uart_base[module], registers::efr::offset);
	io::or_register(registers::ier::sleep_mode, uart_base[module], registers::ier::offset);
	io::and_register(~registers::efr::enhanced_en, uart_base[module], registers::efr::offset);

	// Switch to configuration mode A:
	io::write<char>(0x80, uart_base[module], registers::lcr::offset);

	// Enable the FIFOs:
//	io::write<char>(registers::fcr::fifo_en, uart_base[module], registers::fcr::offset);

	// Set the baud rate divisor for 115.2 kbps transfer rate:
	io::write<char>(0x1a, uart_base[module], registers::dll::offset);
	io::write<char>(0x00, uart_base[module], registers::dlh::offset);

	// Set the transmission format (returning to operational mode):
	io::write<char>(0x3|registers::lcr::parity_en|registers::lcr::parity_type1, uart_base[module], registers::lcr::offset);

	// Enable the IRQ handler:
	irq_handler::get()->register_handler(std::bind(&uart::interrupt_handler, this, std::placeholders::_1),
		IRQ_BASE + module);
	// Enable the data received interrupt, to enable echoing input:
	io::or_register(registers::ier::rhr_it, uart_base[module], registers::ier::offset);

	// Enable UARTx16 mode:
	io::write<char>(0, uart_base[module], registers::mdr1::offset);
}

void uart::interrupt_handler(int irq)
{
	char interrupt_source = (io::read<char>(uart_base[module], registers::iir::offset) >> 1) & 0x1f;

	switch(interrupt_source)
	{
		case 2: // Character received:
			while(((io::read<char>(uart_base[module], registers::iir::offset) >> 1) & 0x1f) == 2)
				kernel::message() << io::read<char>(uart_base[module], registers::rhr::offset)
					<< kstream::newline;
			break;
		case 6: // RX time-out, but seems to work as character received:
			kernel::message() << io::read<char>(uart_base[module], registers::rhr::offset);
			break;
		default:
			break;
	}
}

