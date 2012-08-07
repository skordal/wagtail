// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "sd_card.h"

using namespace wagtail;

// SD card base address:
static void * base_address = reinterpret_cast<void *>(0x4809c000);

// The kernel SD card object:
sd_card * sd_card::sd = nullptr;

// Initializes the SD card driver:
void sd_card::initialize()
{
	sd = new sd_card();
}

// Constructs an SD card object:
sd_card::sd_card() : block_device(device_numbers::sd_card_major, 0, 512, "SD Card"),
	rca(0), virtual_base(mmu::map_device(base_address, 4096))
{
	kernel::message() << "Initializing SD card driver... ";

	// Enable the functional and interface clock:
	clock_mgr::enable(functional_clocks::mmc1);
	clock_mgr::enable(interface_clocks::mmc1);

	// Resets the SD card controller:
	io::or_register(registers::sysconfig::softreset, virtual_base, registers::sysconfig::offset);
	while(!(io::read<int>(virtual_base, registers::sysstatus::offset) & registers::sysstatus::resetdone));

	// Set the hardware capabilities register with the supported voltages;
	// according to the BeagleBoard documentation, the uSD connector is
	// powered by the VMMC1 rail from the TPS65950, which can supply it
	// with 1.8 V and 3.0 V at 220 mA current:
	io::or_register(registers::capa::vs18|registers::capa::vs30, virtual_base, registers::capa::offset);

	io::or_register(registers::con::od, virtual_base, registers::con::offset);

	// Initialize the SD bus:
	io::write(registers::hctl::sdvs(registers::hctl::sd_bus_voltage_30), virtual_base, registers::hctl::offset);

	// Power on the SD bus:
	io::or_register(registers::hctl::sdbp, virtual_base, registers::hctl::offset);
	while(!(io::read<int>(virtual_base, registers::hctl::offset) & registers::hctl::sdbp));

	// Enable the internal clock:
	io::or_register(registers::sysctl::ice, virtual_base, registers::sysctl::offset);

	// Set the clock divisor to attain a < 80 KHz clock signal:
	io::and_register(~(registers::sysctl::clkd_mask|registers::sysctl::cen), virtual_base,
		registers::sysctl::offset);
	io::or_register(registers::sysctl::clkd(0x3ff), virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::ics));
	io::or_register(registers::sysctl::cen, virtual_base, registers::sysctl::offset);

	// Disable wake-up capability for now, otherwise let power settings remain at their default
	// values:
	io::and_register(~registers::sysconfig::enawakeup, virtual_base, registers::sysconfig::offset);

	// Enable status bits:
	int status_bits = registers::ie::cto_enable|registers::ie::tc_enable|
		registers::ie::cc_enable|registers::ie::cerr_enable;
	io::write(status_bits, virtual_base, registers::ie::offset);

	kernel::message() << "ok" << kstream::newline;

	// Initialize the connected SD card:
	initialize_card();
}

// Initializes the connected SD card:
void sd_card::initialize_card()
{
	kernel::message() << "Initializing connected SD card:" << kstream::newline;

	// Send the initialization stream, done twice since the clock value cannot
	// really be set low enough:
	for(int i = 0; i < 2; ++i)
	{
		io::or_register(registers::con::init, virtual_base, registers::con::offset);
		io::write<int>(0, virtual_base, registers::cmd::offset);

		while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc));
		io::or_register(registers::stat::cc, virtual_base, registers::stat::offset);
	}

	// Stop initialization and reset status flags:
	io::and_register(~registers::con::init, virtual_base, registers::con::offset);
	io::write<int>(0xffffffff, virtual_base, registers::stat::offset);

	// Set clock frequency to 400 KHz:
	io::and_register(~(registers::sysctl::cen|registers::sysctl::clkd_mask), virtual_base, registers::sysctl::offset);
	io::or_register(registers::sysctl::clkd(240), virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::ics));
	io::or_register(registers::sysctl::cen, virtual_base, registers::sysctl::offset);

	kernel::message() << "\tBus initialized, starting communication with card:" << kstream::newline;

	// Send CMD0:

	kernel::message() << "\t\tCMD0: ";
	if(send_cmd0())
		kernel::message() << "ok" << kstream::newline;
	else
		kernel::panic();

	// Reset the command line and state machine:
	io::or_register(registers::sysctl::src, virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::src));

	// Send CMD8:
	kernel::message() << "\t\tCMD8: ";
	if(!send_cmd8())
		kernel::panic();

_send_acmd41:
	// Send ACMD41:
	kernel::message() << "\t\tACMD41: ";
	if(!send_acmd41())
		kernel::panic();
	else
		kernel::message() << "ok" << kstream::newline;

	// Repeat until the card is not busy anymore:
	if(!(io::read<int>(virtual_base, registers::rsp10::offset) & (1 << 31)))
		goto _send_acmd41;

	// Send CMD2 to get card CID information (not used yet):
	kernel::message() << "\t\tCMD2: ";
	if(!send_cmd2())
		kernel::panic();
	else
		kernel::message() << "ok" << kstream::newline;

	// Send CMD3 to get the card RCA:
	kernel::message() << "\t\tCMD3: ";
	if(!send_cmd2())
		kernel::panic();
	else
		kernel::message() << "ok" << kstream::newline;

	// TODO: Start reading from the card!
}

// Sends CMD0 to the card:
bool sd_card::send_cmd0()
{
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write<int>(0x00000000, virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(0), virtual_base, registers::cmd::offset);
	return wait_for_cmd_sent();

	return true;
}

// Sends CMD2 to the card:
bool sd_card::send_cmd2()
{
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write<int>(0, virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(2)|registers::cmd::rsp_type(registers::cmd::r2),
		virtual_base, registers::cmd::offset);

	if(!wait_for_cmd_sent())
		return false;

	return true;
}

// Sends CMD3 to the card:
bool sd_card::send_cmd3()
{
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write<int>(0, virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(3)|registers::cmd::rsp_type(registers::cmd::r6),
		virtual_base, registers::cmd::offset);

	if(!wait_for_cmd_sent())
		return false;

	int reply = io::read<int>(virtual_base, registers::rsp10::offset);
	rca = reply >> 16;

	return true;
}

// Sends CMD8 to the card:
bool sd_card::send_cmd8()
{
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write((0b0001 << 8)|(0xaa), virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(8)|registers::cmd::rsp_type(registers::cmd::r7),
		virtual_base, registers::cmd::offset);
	if(!wait_for_cmd_sent())
	{
		if(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cto)
		{
			kernel::message() << "Card may not be SD version 2.0 compliant!" << kstream::newline;
			return true;
		} else
			return false;
	}

	kernel::message() << "Card supports SD version 2.0" << kstream::newline;
	return true;
}

// Sends CMD55 to the card, used by the ACMD methods:
bool sd_card::send_cmd55()
{
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write<int>(0x00000000, virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(55)|registers::cmd::rsp_type(registers::cmd::r1),
		virtual_base, registers::cmd::offset);
	return wait_for_cmd_sent();
}

// Sends ACMD41 to the card:
bool sd_card::send_acmd41()
{
	if(!send_cmd55())
	{
		kernel::message() << "Error sending CMD55, cannot send ACMD41" << kstream::newline;
		return false;
	}

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write<int>((1 << 30)|(1 << 28)|(1 << 17)|(1 << 18), virtual_base, registers::arg::offset);
	io::write(registers::cmd::indx(41)|registers::cmd::rsp_type(registers::cmd::r3),
		virtual_base, registers::cmd::offset);
	if(!wait_for_cmd_sent())
		return false;
	else
		return true;
}

// Waits for a command to finish sending:
bool sd_card::wait_for_cmd_sent()
{
	while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc))
	{
		int status = io::read<int>(virtual_base, registers::stat::offset);
		// Check for time-out:
		if(status & registers::stat::cto)
		{
			kernel::message() << "SD card timeout" << kstream::newline;
			return false;
		} else if(status & registers::stat::cerr)
		{
			kernel::message() << "SD card error" << kstream::newline;
			return false;
		}
	}

	return true;
}

// Reads a block from the SD card:
void sd_card::read_block(void * buffer, block_address_t address)
{

}

// Writes a block to the SD card:
void sd_card::write_block(void * buffer, block_address_t address)
{

}

