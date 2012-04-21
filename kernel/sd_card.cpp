// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "sd_card.h"

using namespace wagtail;

// SD card base address:
void * sd_card::base_address = reinterpret_cast<void *>(0x4809c000);

// The kernel SD card object:
sd_card * sd_card::sd = 0;

void sd_card::initialize()
{
	kernel::message() << "Initializing SD card... ";
	sd = new sd_card();
	kernel::message() << "ok" << kstream::newline;
}

// Constructs an SD card object:
sd_card::sd_card() : block_device(device_numbers::sd_card_major, 0, 512, "SD Card")
{
	virtual_base = mmu::map_device(base_address, 4096);

	// Resets the SD card controller:
	io::or_register(registers::sysconfig::softreset, virtual_base, registers::sysconfig::offset);
	// Wait for reset complete:
	while(!(io::read<int>(virtual_base, registers::sysstatus::offset) & registers::sysstatus::resetdone));

	// Set various configuration parameters:
	io::or_register(registers::sysconfig::sidlemode(registers::sysconfig::idlemode_auto)|registers::sysconfig::autoidle,
		virtual_base, registers::sysconfig::offset);
}

// Reads a block from the SD card:
void sd_card::read_block(void * buffer, block_address_t address)
{

}

// Writes a block to the SD card:
void sd_card::write_block(void * buffer, block_address_t address)
{

}

