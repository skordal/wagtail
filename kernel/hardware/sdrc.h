// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_HARDWARE_SDRC_H
#define WAGTAIL_HARDWARE_SDRC_H

#include "hardware.h"

BEGIN_MODULE(sdrc)
	REGISTER(revision, 0x00)
	REGISTER(sysconfig, 0x10)
	REGISTER(mcfg0, 0x80)
	REGISTER(mcfg1, 0xb0)
END_MODULE()

#endif

