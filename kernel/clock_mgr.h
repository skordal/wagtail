// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_CLOCK_MGR_H
#define WAGTAIL_CLOCK_MGR_H

#include "mmu.h"
#include "io.h"

namespace wagtail
{
	/**
	 * Clock manager class, controlling various functional and interface clocks.
	 */
	class clock_mgr final
	{
		public:
			/** Functional clock names. */
			enum class functional_clocks : unsigned int
			{
				mmc3 = 1 << 30,
				mmc2 = 1 << 25,
				mmc1 = 1 << 24
			};

			/** Interface clock names. */
			enum class interface_clocks : unsigned int
			{
				mmc3 = 1 << 30,
				mmc2 = 1 << 25, 
				mmc1 = 1 << 24
			};

			/**
			 * Enables a functional clock.
			 * @param clock the clock to enable.
			 */
			static void enable(functional_clocks clock);
			/**
			 * Enables an interface clock.
			 * @param clock the clock to enable.
			 */
			static void enable(interface_clocks clock);

			/**
			 * Disables a functional clock.
			 * @param clock the clock to disable.
			 */
			static void disable(functional_clocks clock);
			/**
			 * Disables an interface clock.
			 * @param clock the clock to disable.
			 */
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

