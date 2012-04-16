// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "process.h"

using namespace wagtail;

// Constructs a new process from the specified memory buffer:
process::process(void * buffer)
{
	translation_table = new (8 * 1024) mmu::application_translation_table_t;
}

