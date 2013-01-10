// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_HARDWARE_TIMER_H
#define WAGTAIL_HARDWARE_TIMER_H

#include "hardware.h"

BEGIN_MODULE(timer)
	REGISTER(tidr, 0x00)

	REGISTER_BEGIN(tiocp_cfg, 0x10)
		BITNAME(softreset, 1)
	REGISTER_END()

	REGISTER_BEGIN(tistat, 0x14)
		BITNAME(resetdone, 0)
	REGISTER_END()

	REGISTER_BEGIN(tisr, 0x18)
		BITNAME(mat_it_flag, 0)
		BITNAME(ovf_it_flag, 1)
		BITNAME(tcar_it_flag, 2)
	REGISTER_END()

	REGISTER_BEGIN(tier, 0x1c)
		BITNAME(mat_it_ena, 0)
		BITNAME(ovf_it_ena, 1)
		BITNAME(tcar_it_ena, 2)
	REGISTER_END()

	REGISTER_BEGIN(tclr, 0x24)
		BITNAME(st, 0)
		BITNAME(ar, 1)

		BITFIELD(trg, 10, 2)
	REGISTER_END()

	REGISTER(tcrr, 0x28)
	REGISTER(tldr, 0x2c)
	REGISTER(ttgr, 0x30)
END_MODULE()

#endif

