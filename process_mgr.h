// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_MGR_H
#define WAGTAIL_PROCESS_MGR_H

#include "mmu.h"
#include "process.h"

namespace wagtail
{
	class process_mgr
	{
		public:
			// Initializes the process manager:
			static void initialize();

		private:
			// Activates the specified process:
			void activate(const process & proc);

			// The kernel process manager object:
			static process_mgr * global_process_mgr;
	};
}

#endif

