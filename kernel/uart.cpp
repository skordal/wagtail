// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "uart.h"

using namespace wagtail;

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
	while(!(io::read<char>(uart_base[module], uart::registers::lsr::offset)
		& uart::registers::lsr::tx_fifo_e));
	// Write the character to the UART:
	io::write(character, uart_base[module], uart::registers::thr::offset);
	
	return *this;
}

// Reads a character from a UART module:
kistream & uart::operator >> (char & character)
{
	// Wait for the UART to be ready to send:
	while(!(io::read<char>(uart_base[module], uart::registers::lsr::offset)
		& uart::registers::lsr::rx_fifo_e));
	character = io::read<char>(uart_base[module], uart::registers::rhr::offset);
	return *this;
}

// Constructs a new UART module with the specified parameters:
uart::uart(int module) : character_device(device_numbers::uart_major, module, "UART"), module(module)
{
}

