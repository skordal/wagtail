// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "device.h"
#include "kthread.h"

using namespace wagtail;

bool block_device::read_block(void * destination, block_address_t address)
{
	block_read_operation * op = new block_read_operation(destination, 1, address);
	post_read(op);
	kthread::wait_for_io(op);
	delete op;

	return false;
}

bool block_device::read_blocks(void * destination, block_address_t address, unsigned int length)
{
	block_read_operation * op = new block_read_operation(destination, length, address);
	post_read(op);
	kthread::wait_for_io(op);
	delete op;

	return false;
}

