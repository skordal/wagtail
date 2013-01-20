// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#include "kernel.h"
#include "waitblock.h"

using namespace wagtail;

waitblock::waitblock(unsigned int pid, unsigned int * status_loc, process * caller)
	: process_block(), termination_listener(), waitpid(pid), status_loc(status_loc), caller(caller)
{
	if(waitpid == -1U || waitpid == 0)
	{
		if(caller->get_num_children() != 0)
			caller->set_child_termination_listener(this);
		else {
			active = false;
			terminated_pid = -1;
		}
	} else {
		process * p = scheduler::get()->get_process(pid);
		if(p == nullptr)
		{
			active = false;
			terminated_pid = -1;
		} else
			p->add_termination_listener(this);
	}
}

waitblock::~waitblock()
{
	if(waitpid == -1U || waitpid == 0)
		caller->set_child_termination_listener(nullptr);
}

void waitblock::block_return()
{
	// Set the return value of the wait system call to the terminated process PID:
	caller->get_registers().r[0] = terminated_pid;

	// Write the exit code to the specified memory location:
	if(status_loc != nullptr)
		*status_loc = exit_code;
}

void waitblock::process_terminated(process * terminated)
{
	terminated_pid = terminated->get_pid();
	exit_code = terminated->get_exit_code();

	active = false;
}

