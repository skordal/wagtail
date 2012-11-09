// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "sd.h"

using namespace wagtail;
using namespace wagtail::hardware::sd;

sd * sd::sd_module = nullptr;

void sd::initialize()
{
	sd_module = new sd;
}

sd::sd() : block_device(512, "sd"), virtual_base(mmu::map_device(sd::BASE_ADDRESS, 4096))
{
	kernel::message() << "Initializing SD module:" << kstream::newline;

	kernel::message() << "\tEnabling clocks: ";
	clock_mgr::enable(functional_clocks::mmc1);
	clock_mgr::enable(interface_clocks::mmc1);
	kernel::message() << "ok" << kstream::newline;

	kernel::message() << "\tResetting: ";
	io::write(registers::sysconfig::softreset, virtual_base, registers::sysconfig::offset);
	while(!(io::read<int>(virtual_base, registers::sysstatus::offset) & registers::sysstatus::resetdone));

	io::or_register(registers::sysctl::sra, virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::sra));
	kernel::message() << "ok" << kstream::newline;

	kernel::message() << "\tSetting capability registers: ";
	io::or_register(registers::capa::vs30|registers::capa::vs18, virtual_base, registers::capa::offset);
	io::write(220 << registers::cur_capa::cur_1v8|220 << registers::cur_capa::cur_3v0, virtual_base,
		registers::cur_capa::offset);
	kernel::message() << "ok" << kstream::newline;

	kernel::message() << "\tInitializing SD bus: ";

	kernel::message() << io::read<void *>(virtual_base, registers::sysctl::offset) << " ";

	// Enable power on the SD bus:
	io::write(0x6 << registers::hctl::sdvs, virtual_base, registers::hctl::offset);
	io::or_register(registers::hctl::sdbp, virtual_base, registers::hctl::offset);
	while(!(io::read<int>(virtual_base, registers::hctl::offset) & registers::hctl::sdbp));

	// Enable the interface clock:
	io::or_register(registers::sysctl::ice, virtual_base, registers::sysctl::offset);

	// Enable the SD bus clock:
	io::and_register(~(0x3ff << registers::sysctl::clkd|registers::sysctl::cen),
		virtual_base, registers::sysctl::offset);
	io::or_register(0x3ff << registers::sysctl::clkd, virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::ics));
	io::or_register(registers::sysctl::cen, virtual_base, registers::sysctl::offset);

	kernel::message() << io::read<void *>(virtual_base, registers::sysctl::offset) << " ";
	kernel::message() << "ok" << kstream::newline;

	// TODO: handle checking for card present and ejecting and inserting cards.
	initialize_card();
}

void sd::initialize_card()
{
	kernel::message() << "Initializing SD card: " << kstream::newline;

	kernel::message() << "\tSending initialization stream: ";
	io::write(registers::ie::cc, virtual_base, registers::ie::offset);
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::or_register(registers::con::init, virtual_base, registers::con::offset);
	for(int i = 0; i < 3; ++i)
	{
		io::write(0, virtual_base, registers::cmd::offset);
		while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc));
		io::write(0xffffffff, virtual_base, registers::stat::offset);
	}
	io::and_register(~registers::con::init, virtual_base, registers::con::offset);
	kernel::message() << "ok" << kstream::newline;

	// FIXME: At this point the bus clock frequency is ~94 KHz, it should be 400 KHz.
	// FIXME: Switching to 400 KHz (or anything else, for that matter), seems to cause
	// FIXME: problems when issuing CMD2 and onwards.

	// Send CMD0:
	send_cmd0();

	// Send CMD8:
	if(!send_cmd8())
		return;

	// Send ACMD41:
	int acmd41_result;
	do {
		acmd41_result = send_acmd41();
		if(acmd41_result == -1)
			return;
	} while(!(acmd41_result & (1 << 31))); // Loop while busy.

	// Send CMD2:
	if(!send_cmd2())
		return;

	// Send CMD3:
	if(!send_cmd3())
		return;

	card_initialized = true;
}

void sd::send_cmd0()
{
	kernel::message() << "\tCMD0: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc, virtual_base, registers::ie::offset);

	io::write(0, virtual_base, registers::arg::offset);
	io::write(0, virtual_base, registers::cmd::offset);

	while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc));
	kernel::message() << "ok" << kstream::newline;
}

bool sd::send_cmd2()
{
	kernel::message() << "\tCMD2: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto|registers::ie::cerr,
		virtual_base, registers::ie::offset);

	io::write(0, virtual_base, registers::arg::offset);
	io::write(2 << registers::cmd::indx|1 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << kstream::newline;
		return false;
	}

	kernel::message() << "ok" << kstream::newline;

	// Just print the CID, as it's not used for anything yet.
	kernel::message() << "\t -> CID is "
		<< io::read<void *>(virtual_base, registers::rsp76::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp54::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp32::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp10::offset) << " "
		<< kstream::newline;

	return true;
}

bool sd::send_cmd3()
{
	kernel::message() << "\tCMD3: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(0, virtual_base, registers::arg::offset);
	io::write(3 << registers::cmd::indx|0x2 << registers::cmd::rsp_type, virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << kstream::newline;
		return false;
	}

	rca = io::read<int>(virtual_base, registers::rsp10::offset) >> 16;
	kernel::message() << "ok" << kstream::newline;
	kernel::message() << "\t -> card RCA is " << (int) rca << kstream::newline;

	return true;
}

bool sd::send_cmd8()
{
	kernel::message() << "\tCMD8: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(1 << 8|0xaa, virtual_base, registers::arg::offset);
	io::write(8 << registers::cmd::indx|0x2 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << kstream::newline;
		return false;
	}

	int reply = io::read<int>(virtual_base, registers::rsp10::offset);
	if((reply & 0xaa) != 0xaa)
	{
		kernel::message() << "check pattern mismatch" << kstream::newline;
		return false;
	} else if(!(reply & (1 << 8)))
	{
		kernel::message() << "operating condition not accepted" << kstream::newline;
		return false;
	}

	kernel::message() << "ok" << kstream::newline;
	return true;
}

bool sd::send_cmd55()
{
	kernel::message() << "\t(CMD55: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(rca << 16, virtual_base, registers::arg::offset);
	io::write(55 << registers::cmd::indx|0x2 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << ")" << kstream::newline;
		return false;
	} else {
		kernel::message() << "ok)" << kstream::newline;
		return true;
	}
}

int sd::send_acmd41()
{
	if(!send_cmd55())
		return -1;

	kernel::message() << "\tACMD41: ";

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(1 << 30|1 << 28|1 << 18|1 << 17, virtual_base, registers::arg::offset);
	io::write(41 << registers::cmd::indx|0x2 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << kstream::newline;
		return -1;
	}

	kernel::message() << "ok" << kstream::newline;
	return io::read<int>(virtual_base, registers::rsp10::offset);
}

sd::sd_status sd::wait_for_status()
{
	int status;
	do {
		status = io::read<int>(virtual_base, registers::stat::offset);
		if(status & registers::stat::ccrc)
			return sd_status::command_crc_error;
		if(status & registers::stat::cerr)
			return sd_status::card_error;
		if(status & registers::stat::cie)
			return sd_status::command_index_error;
		if(status & registers::stat::cto)
			return sd_status::command_timeout;
	} while(!(status & registers::stat::cc));
	return sd_status::command_complete;
}

const char * sd::get_error_message(sd::sd_status status) const
{
	switch(status)
	{
		case sd_status::card_error:
			return "card error";
		case sd_status::command_crc_error:
			return "command CRC error";
		case sd_status::command_index_error:
			return "command index error";
		case sd_status::command_timeout:
			return "command timeout error";
		case sd_status::command_complete:
			return "command complete";
		default:
			return "unknown error";
	}
}

