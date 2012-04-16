// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_SDRC_H
#define WAGTAIL_SDRC_H

namespace wagtail
{
	namespace sdrc
	{
		// SDRC module base address:
		static void * const base = reinterpret_cast<void *>(0x6d000000);

		namespace registers
		{
			static const unsigned int revision = 0x00;
			static const unsigned int sysconfig = 0x10;
			static const unsigned int mcfg[2] = {0x80, 0xb0};
		}
	}
	
}

#endif

