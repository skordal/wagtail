// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "partition.h"

using namespace wagtail;

partition::partition(block_device * device, block_address_t start_addr, block_address_t end_addr, char type, int num)
	: block_device(device->get_block_size(), name = partname(device->get_name(), num)),
	  device(device), start(start_addr), end(end_addr), type(type)
{
	kernel::message() << "Partition registered: " << name << kstream::newline;
}

// FIXME: The I/O methods below assumes the block address fits within 32 bits. This is
// FIXME: always the case on the Beagleboard, but block_address_t allows 64 bit addresses,
// FIXME: so this should be fixed some time.

bool partition::read_block(void * buffer, block_address_t address)
{
	if(address < start || address > end)
	{
		kernel::message() << name << ": cannot read from address " << (void *) address
			<< " it is outside the partition!" << kstream::newline;
		return false;
	}

	return device->read_block(buffer, address);
}

bool partition::read_blocks(void * buffer, block_address_t address, int length)
{
	if(address < start || address > end)
	{
		kernel::message() << name << ": cannot read from address " << (void *) address
			<< " it is outside the partition!" << kstream::newline;
		return false;
	}

	return device->read_blocks(buffer, address, length);
}

bool partition::write_block(const void * buffer, block_address_t address)
{
	if(address < start || address > end)
	{
		kernel::message() << name << ": cannot write to address " << (void *) address
			<< " it is outside the partition!" << kstream::newline;
		return false;
	}

	return device->write_block(buffer, address);
}

bool partition::write_blocks(const void * buffer, block_address_t address, int length)
{
	if(address < start || address > end)
	{
		kernel::message() << name << ": cannot write to address " << (void *) address
			<< " it is outside the partition!" << kstream::newline;
		return false;
	}

	return device->write_blocks(buffer, address, length);
}

char * partition::partname(const char * basename, int partnum)
{
	char * buffer = new char[utils::strlen(basename) + 3];
	utils::strcpy(buffer, basename);
	utils::strcat(buffer, "p");

	switch(partnum)
	{
		case 0:
			utils::strcat(buffer, "0");
			break;
		case 1:
			utils::strcat(buffer, "1");
			break;
		case 2:
			utils::strcat(buffer, "2");
			break;
		case 3:
			utils::strcat(buffer, "3");
			break;
		default:
			utils::strcat(buffer, "!");
			break;
	}

	return buffer;
}

