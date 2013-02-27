// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "io_operation.h"
#include "process.h"
#include "scheduler.h"

using namespace wagtail;

io_operation::io_operation(process * owner, std::function<void(io_operation *, status)> callback)
	: owner(owner), callback(callback)
{

}

void io_operation::complete(status stat)
{
	this->stat = stat;
	if(callback != nullptr)
		callback(this, stat);
	else
		delete this;
}

read_operation::read_operation(void * destination, unsigned int size, unsigned int offset,
	process * owner, std::function<void(io_operation *, status)> callback)
	: io_operation(owner, callback), destination(destination), size(size), offset(offset)
{

}

kostream & wagtail::operator<<(kostream & out, const read_operation & op)
{
	out << "{read operation: [dest = " << op.destination << "] [size = " << (int) op.size << "] "
		<< "[offset = " << (void *) op.offset << "]}";

	return out;
}

write_operation::write_operation(const void * source, unsigned int size, unsigned int offset,
	process * owner, std::function<void(io_operation *, status)> callback)
	: io_operation(owner, callback), source(source), size(size), offset(offset)
{

}

unsigned char write_operation::operator[](unsigned int index) const
{
	if(get_owner() != nullptr)
	{
		unsigned char retval;
		mmu::set_application_table(get_owner()->get_translation_table(), get_owner()->get_pid());
		retval = ((const unsigned char *) source)[index];
		mmu::set_application_table(scheduler::get()->get_current_process()->get_translation_table(),
			scheduler::get()->get_current_process()->get_pid());
		return retval;
	} else
		return ((const unsigned char *) source)[index];
}

block_read_operation::block_read_operation(void * destination, unsigned int blocks, unsigned int address,
	process * owner, std::function<void(io_operation *, status)> callback)
	: io_operation(owner, callback), destination(destination), blocks(blocks), address(address)
{

}

kostream & wagtail::operator<<(kostream & out, const block_read_operation & op)
{
	out << "{block read operation: [dest = " << op.destination << "] [blocks = " << (int) op.blocks << "] "
		<< "[address = " << (void *) op.address << "]}";
	return out;
}

