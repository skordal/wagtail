// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "process_mgr.h"

using namespace wagtail;

// The global process manager:
process_mgr * process_mgr::global_process_mgr = 0;

// Initializes the process manager:
void process_mgr::initialize()
{
	global_process_mgr = new process_mgr;
}

