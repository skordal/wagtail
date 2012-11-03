// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_HARDWARE_INTC_H
#define WAGTAIL_HARDWARE_INTC_H

#include "hardware.h"

BEGIN_MODULE(intc)
	REGISTER(revision,		0x000)
	REGISTER(sir_irq,		0x040)
	REGISTER(threshold,		0x068)
	REGISTER(mir_clear_base,	0x088)
	REGISTER(mir_set_base,		0x08c)
	REGISTER(isr_set_base,		0x090)
	REGISTER(isr_clear_base,	0x094)
	REGISTER(ilr_base,		0x100)

	REGISTER_BEGIN(control, 0x48)
		BITNAME(newirqagr, 0)
		BITNAME(newfiqagr, 1)
	REGISTER_END()

	REGISTER_BEGIN(protection, 0x04c)
		BITNAME(enable, 0)
	REGISTER_END()
END_MODULE()

#endif

