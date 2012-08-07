// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_CLOCK_MGR_H
#define WAGTAIL_CLOCK_MGR_H

#include "mmu.h"
#include "io.h"

namespace wagtail
{
	// Functional clocks that can be enabled or disabled:
	enum class functional_clocks : unsigned int
	{
		mmc3 = 1 << 30,
		mmc2 = 1 << 25,
		mmc1 = 1 << 24
	};

	// Interface clocks that can be enabled or disabled:
	enum class interface_clocks : unsigned int
	{
		mmc3 = 1 << 30,
		mmc2 = 1 << 25, 
		mmc1 = 1 << 24
	};

	// This class controls clocks for various modules in the device
	// using the PRCM's clock module:
	class clock_mgr
	{
		public:
			// Enables a clock:
			static void enable(functional_clocks clock);
			static void enable(interface_clocks clock);

			// Disables a clock:
			static void disable(functional_clocks clock);
			static void disable(interface_clocks clock);
		private:
			static void * core_cm_base;

			struct fclken1_core
			{
				static const unsigned int offset = 0x00;
			};

			struct iclken1_core
			{
				static const unsigned int offset = 0x10;
			};
	};
}

#endif

