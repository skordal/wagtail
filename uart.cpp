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

// UART module objects:
uart uart::uart_modules[4] = {
	uart(0), uart(1), uart(2), uart(3)
};

// Prints a debug message to UART2. This is used early in the initialization
// process to print debug messages. Note that this function cannot be used
// after the MMU has been initialized.
/*void uart::early_print_string(const char * string)
{
	const void * debug_base = reinterpret_cast<void *>(0x49020000);

	for(int index = 0; string[index] != 0; ++index)
	{
		// For newlines, also print a \r to move the cursor to the beginning
		// of the line:
		if(string[index] == '\n')
			uart::early_print_string("\r");

		// Wait for the UART to be ready to send:
		while(!(io::read<char>(debug_base, uart::registers::lsr_reg)
			& uart::registers::lsr::tx_fifo_e));

		// Write the character to the UART:
		io::write(string[index], debug_base, uart::registers::thr_reg);
	}
}*/

// Writes a character to a UART module:
kostream & uart::operator << (char character)
{
	// For newlines, a \r must also be printed:
	if(character == '\n')
		*this << '\r';

	// Wait for the UART to be ready to send:
	while(!(io::read<char>(uart_base[module], uart::registers::lsr_reg)
		& uart::registers::lsr::tx_fifo_e));
	// Write the character to the UART:
	io::write(character, uart_base[module], uart::registers::thr_reg);
	
	return *this;
}

// Reads a character from a UART module:
kistream & uart::operator >> (char & character)
{
	// Wait for the UART to be ready to send:
	while(!(io::read<char>(uart_base[module], uart::registers::lsr_reg)
		& uart::registers::lsr::rx_fifo_e));
	character = io::read<char>(uart_base[module], uart::registers::rhr_reg);
	return *this;
}

// Constructs a new UART module with the specified parameters:
uart::uart(int module) : character_device(device_numbers::uart_major, module, "UART module"), module(module)
{
}

