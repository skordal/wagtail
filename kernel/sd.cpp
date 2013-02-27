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

block_read_operation * sd::post_read(block_read_operation * read_op)
{
	read_op->set_status(io_operation::status::queued);
	read_queue.push_back(read_op);
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": operation queued: " << *read_op << kstream::newline;
#endif

	if(active_read == nullptr)
		initiate_next_read();
	return read_op;
}

block_write_operation * sd::post_write(block_write_operation * write_op)
{
	write_op->complete(io_operation::status::failed);
	return write_op;
}

sd::sd() : block_device(512, "sd0"), virtual_base(mmu::map_device(sd::BASE_ADDRESS, 4096))
{
	kernel::message() << "Initializing SD module..." << kstream::newline;

	clock_mgr::enable(clock_mgr::functional_clocks::mmc1);
	clock_mgr::enable(clock_mgr::interface_clocks::mmc1);

	io::write(registers::sysconfig::softreset, virtual_base, registers::sysconfig::offset);
	while(!(io::read<int>(virtual_base, registers::sysstatus::offset) & registers::sysstatus::resetdone));

	io::or_register(registers::sysctl::sra, virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::sra));

	io::or_register(registers::capa::vs30|registers::capa::vs18, virtual_base, registers::capa::offset);
	io::write(220 << registers::cur_capa::cur_1v8|220 << registers::cur_capa::cur_3v0, virtual_base,
		registers::cur_capa::offset);

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

	// Set the block length register:
	io::write<int>(0x200, virtual_base, registers::blk::offset);

	// Set the module IRQ handler:
	irq_handler::get()->register_handler(std::bind(&sd::irq_handler, this, std::placeholders::_1), 83);

	// TODO: handle checking for card present and ejecting and inserting cards.
	initialize_card();
}

void sd::initialize_card()
{
	kernel::message() << "Initializing SD card..." << kstream::newline;

	// Send the initialization stream:
	switch_clock_divider(0x3ff); // Gives a frequency of 93.841 KHz, the lowest attainable.
	io::write(registers::ie::cc, virtual_base, registers::ie::offset);
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::or_register(registers::con::init, virtual_base, registers::con::offset);
	for(int i = 0; i < 3; ++i) // Continue sending the initialization stream for a while
	                           // due to the too high clock frequency.
	{
		io::write(0, virtual_base, registers::cmd::offset);
		while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc));
		io::write(0xffffffff, virtual_base, registers::stat::offset);
	}
	io::and_register(~registers::con::init, virtual_base, registers::con::offset);

	// Switch the clock frequency to 400 KHz:
	switch_clock_divider(240);

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

	// Select/enable the card, send CMD7:
	if(!send_cmd7())
		return;

	// Change to 4-bit mode if ACMD6 is successful:
	if(send_acmd6())
		io::or_register(registers::hctl::dtw, virtual_base, registers::hctl::offset);

	// Clear all status flags before enabling the interrupts:
	io::write(0xffffffff, virtual_base, registers::stat::offset);
	// Enable SD card interrupts:
	io::write(0xffffffff, virtual_base, registers::ise::offset);

	// Read the partition table to finish the initialization:
	read_partition_table();
}

void sd::read_partition_table()
{
	// Read the card's MBR from the first block of the card:
	char * buffer = new char[512];
	block_read_operation * mbr_read = new block_read_operation(buffer, 1, 0);
	post_read(mbr_read);

	// Wait until the read completes:
	kthread::wait_for_io(mbr_read);
	delete mbr_read;

	for(int i = 0; i < 4; ++i)
	{
		block_address_t start_address, end_address;
		char type = buffer[446 + 4 + (i * 16)];

		// Due to the start and end addresses being split between two
		// words in memory, the following is neccessary to assemble the
		// addresses byte for byte:
		start_address =
			(*((unsigned int *)(buffer + 446 + 8 + (i * 16) - 2)) >> 16) |
			(*((unsigned int *)(buffer + 446 + 8 + (i * 16) + 2)) << 16);
		end_address =
			((*((unsigned int *)(buffer + 446 + 12 + (i * 16) - 2)) >> 16) |
			(*((unsigned int *)(buffer + 446 + 12 + (i * 16) + 2)) << 16))
			+ start_address;

		if(type != 0)
			partitions[i] = new partition(this, start_address, end_address, type, i);
	}

	delete[] buffer;
}

void sd::switch_clock_divider(short divider)
{
	int sysctl_register = io::read<int>(virtual_base, registers::sysctl::offset);
	sysctl_register &= ~(0x3ff << registers::sysctl::clkd|registers::sysctl::cen);
	sysctl_register |= 240 << registers::sysctl::clkd;

	io::write(sysctl_register, virtual_base, registers::sysctl::offset);
	while(!(io::read<int>(virtual_base, registers::sysctl::offset) & registers::sysctl::ics));
	io::or_register(registers::sysctl::cen, virtual_base, registers::sysctl::offset);
}

void sd::send_cmd0()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD0" << kstream::newline;
#endif

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc, virtual_base, registers::ie::offset);

	io::write(0, virtual_base, registers::arg::offset);
	io::write(0, virtual_base, registers::cmd::offset);

	while(!(io::read<int>(virtual_base, registers::stat::offset) & registers::stat::cc));
}

bool sd::send_cmd2()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD2" << kstream::newline;
#endif

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto|registers::ie::cerr,
		virtual_base, registers::ie::offset);

	io::write(0, virtual_base, registers::arg::offset);
	io::write(2 << registers::cmd::indx|1 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_name() << ": CMD2: " << get_error_message(status)
			<< kstream::newline;
		return false;
	}

#ifdef WAGTAIL_SD_DEBUG
	// Just print the CID, as it's not used for anything yet.
	kernel::message() << get_name() << ": CMD2: card CID register is "
		<< io::read<void *>(virtual_base, registers::rsp76::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp54::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp32::offset) << " "
		<< io::read<void *>(virtual_base, registers::rsp10::offset) << " "
		<< kstream::newline;
#endif

	return true;
}

bool sd::send_cmd3()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD3" << kstream::newline;
#endif

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

#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD3: card RCA is " << (int) rca << kstream::newline;
#endif

	return true;
}

bool sd::send_cmd7()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD7" << kstream::newline;
#endif

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(rca << 16, virtual_base, registers::arg::offset);
	io::write(7 << registers::cmd::indx|0x3 << registers::cmd::rsp_type, virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_name() << ": CMD7: " << get_error_message(status)
			<< kstream::newline;
		return false;
	}

	return true;
}

bool sd::send_cmd8()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD8" << kstream::newline;
#endif

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
		kernel::message() << get_name() << ": CMD8: check pattern mismatch"
			<< kstream::newline;
		return false;
	} else if(!(reply & (1 << 8)))
	{
		kernel::message() << get_name() << ": CMD8: operating condition not accepted"
			<< kstream::newline;
		return false;
	}

	return true;
}

bool sd::send_cmd55()
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD55" << kstream::newline;
#endif

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(rca << 16, virtual_base, registers::arg::offset);
	io::write(55 << registers::cmd::indx|0x2 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_name() << ": CMD55: " << get_error_message(status)
			<< kstream::newline;
		return false;
	} else {
		return true;
	}
}

bool sd::send_acmd6()
{
	if(!send_cmd55())
		return false;

#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": ACMD6" << kstream::newline;
#endif

	io::write(0xffffffff, virtual_base, registers::stat::offset);
	io::write(registers::ie::cc|registers::ie::cto, virtual_base, registers::ie::offset);

	io::write(0x00000002, virtual_base, registers::arg::offset);
	io::write(6 << registers::cmd::indx|0x2 << registers::cmd::rsp_type,
		virtual_base, registers::cmd::offset);

	sd_status status = wait_for_status();
	if(status != sd_status::command_complete)
	{
		kernel::message() << get_error_message(status) << kstream::newline;
		return false;
	}

	return true;
}

// Returns the reply data, or -1 if an error occurs:
int sd::send_acmd41()
{
	if(!send_cmd55())
		return -1;

#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": ACMD41" << kstream::newline;
#endif

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

	return io::read<int>(virtual_base, registers::rsp10::offset);
}

void sd::send_cmd17(block_address_t address)
{
#ifdef WAGTAIL_SD_DEBUG
	kernel::message() << get_name() << ": CMD17" << kstream::newline;
#endif
	// Set the enabled interrupts:
	io::write(registers::ie::cc|registers::ie::tc|registers::ie::cto|registers::ie::brr|
		registers::ie::dcrc|registers::ie::deb|registers::ie::dto,
		virtual_base, registers::ie::offset);

	// Send the command:
	io::write(static_cast<unsigned int>(address), virtual_base, registers::arg::offset);
	io::write(17 << registers::cmd::indx|0x2 << registers::cmd::rsp_type|registers::cmd::dp|
		registers::cmd::ccce|registers::cmd::ddir, virtual_base, registers::cmd::offset);
}

sd::sd_status sd::wait_for_status()
{
	int status;
	do {
		status = io::read<int>(virtual_base, registers::stat::offset);
		if(status & registers::stat::dcrc)
			return sd_status::data_crc_error;
		if(status & registers::stat::dto)
			return sd_status::data_timeout_error;
		if(status & registers::stat::deb)
			return sd_status::data_end_bit_error;
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

void sd::initiate_read(block_read_operation * read_op)
{
	read_op->set_status(io_operation::status::incomplete);
	active_read = read_op;
	read_attempts = 0;

	send_cmd17(read_op->get_address());
}

void sd::irq_handler(int irq)
{
	if(irq != 83)
		return;

	// If no read is active, ignore the interrupt and clear all status flags:
	if(active_read == nullptr)
	{
#ifdef WAGTAIL_SD_DEBUG
		kernel::message() << get_name() << ": warning: SD interrupt handler called with no active operation!"
			<< " [status = " << io::read<void *>(virtual_base, registers::stat::offset) << "]"
			<< kstream::newline;
#endif
		io::write(0xffffffff, virtual_base, registers::stat::offset);
	} else {
		unsigned int status = io::read<unsigned int>(virtual_base, registers::stat::offset);
#ifdef WAGTAIL_SD_DEBUG
		kernel::message() << get_name() << ": SD interrupt, status register = " << (void *) status << kstream::newline;
#endif
		if(status & registers::stat::brr)
		{
#ifdef WAGTAIL_SD_DEBUG
			kernel::message() << get_name() << ": reading data... ";
#endif
			for(int i = 0; io::read<int>(virtual_base, registers::pstate::offset) & registers::pstate::bre; ++i)
			{
				unsigned int * buffer = static_cast<unsigned int *>(active_read->get_destination());
				buffer[i + active_read->get_blocks_read() * get_block_size()] =
					io::read<unsigned int>(virtual_base, registers::data::offset);
			}
#ifdef WAGTAIL_SD_DEBUG
			kernel::message() << "ok" << kstream::newline;
#endif
			active_read->add_blocks_read(1);
			io::write(registers::stat::brr, virtual_base, registers::stat::offset);
		} else if(status & registers::stat::tc)
		{
#ifdef WAGTAIL_SD_DEBUG
			kernel::message() << get_name() << ": transfer complete!" << kstream::newline;
#endif
			if(active_read->get_blocks_read() < active_read->get_blocks())
			{
				send_cmd17(active_read->get_address() + active_read->get_blocks_read());
				io::write(registers::stat::tc, virtual_base, registers::stat::offset);
			} else {
				active_read->complete(io_operation::status::successful);
				active_read = nullptr;
				io::write(registers::stat::tc, virtual_base, registers::stat::offset);
				initiate_next_read();
			}

		} else if(status & registers::stat::cc)
		{
#ifdef WAGTAIL_SD_DEBUG
			kernel::message() << get_name() << ": command complete!" << kstream::newline;
#endif
			io::write(registers::stat::cc, virtual_base, registers::stat::offset);
		}
	}
}

void sd::initiate_next_read()
{
	block_read_operation * next = nullptr;
	if(read_queue.pop_front(next))
		initiate_read(next);
}

