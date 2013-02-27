// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "partition.h"

using namespace wagtail;

partition::partition(block_device * device, block_address_t start_addr, block_address_t end_addr, char type, int num)
	: block_device(device->get_block_size(), partname(device->get_name(), num)),
	  device(device), start(start_addr), end(end_addr), type(type)
{
	kernel::message() << "Partition registered: " << get_name() << kstream::newline;
}

block_read_operation * partition::post_read(block_read_operation * op)
{
	return device->post_read(op);
}

block_write_operation * partition::post_write(block_write_operation * op)
{
	return device->post_write(op);
}

kstring partition::partname(const kstring & basename, int partnum)
{
	kstring retval = basename;
	retval += "p";

	switch(partnum)
	{
		case 0:
			retval += "0";
			break;
		case 1:
			retval += "1";
			break;
		case 2:
			retval += "2";
			break;
		case 3:
			retval += "3";
			break;
		default:
			retval += "!";
			break;
	}

	return retval;
}

