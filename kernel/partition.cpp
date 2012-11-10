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

