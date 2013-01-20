// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_TERMINATION_LISTENER_H
#define WAGTAIL_TERMINATION_LISTENER_H

namespace wagtail
{
	class process;

	/**
	 * Interface for listening for process termination.
	 * A class derived from this class can use `process::add_termination_listener` to
	 * add itself as a termination listener for the specified process. It will then
	 * be notified when the process terminates.
	 */
	class termination_listener
	{
		public:
			virtual void process_terminated(process *) = 0;
	};
}

#endif

