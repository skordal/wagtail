// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SCM_H
#define WAGTAIL_SCM_H

#include "mmu.h"

namespace wagtail
{
	// Driver for the system control module
	class scm
	{
		public:
			class padconf_register
			{
				public:
					static const unsigned int mmc1_clk = 0x114 / 4;

					void enable_input_0(bool enable = true);
					void enable_input_1(bool enable = true);
				private:
					volatile unsigned int register_value;
			};
			static_assert(sizeof(padconf_register) == 4, "padconf_register must be 32 bits long");

			static scm * get() { return global_scm; }
			static void initialize();

			// Gets the specified PADCONF register:
			padconf_register * get_padconf_register(unsigned int index);
		private:
			scm();

			padconf_register * padconfs;
			void * virtual_base;

			static scm * global_scm;
	};
}

#endif

