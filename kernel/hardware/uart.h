// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_HARDWARE_UART_H
#define WAGTAIL_HARDWARE_UART_H

#include "hardware.h"

BEGIN_MODULE(uart)
	REGISTER(thr, 0x00)
	REGISTER(rhr, 0x00)
	REGISTER(dll, 0x00)
	REGISTER(dlh, 0x04)

	REGISTER_BEGIN(lcr, 0x0c)
		BITNAME_8(parity_en, 3)
		BITNAME_8(parity_type1, 4)
	REGISTER_END()

	REGISTER_BEGIN(fcr, 0x08)
		BITNAME_8(fifo_en, 0)
	REGISTER_END()

	REGISTER_BEGIN(lsr, 0x14)
		BITNAME_8(rx_fifo_e, 0)
		BITNAME_8(tx_fifo_e, 5)
	REGISTER_END()

	REGISTER(mdr1, 0x20)

	REGISTER_BEGIN(ssr, 0x44)
		BITNAME_8(tx_fifo_full, 0)
	REGISTER_END()

	REGISTER_BEGIN(sysc, 0x54)
		BITNAME_8(autoidle, 0)
		BITNAME_8(softreset, 1)

		enum idlemode_t
		{
			force = 0,
			no = 1,
			smart = 2
		};

		static constexpr int idlemode(idlemode_t x) { return x << 3; }
	REGISTER_END()

END_MODULE()

#endif

