// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_REGISTER_CONTENTS_H
#define WAGTAIL_REGISTER_CONTENTS_H

#include "kstream.h"
#include "asm/processor.h"

namespace wagtail
{
	/**
	 * Structure containing ARM register contents.
	 */
	struct register_contents
	{
		/** Registers r0 - r14. */
		unsigned int r[15];
		/** The CPSR of the process. */
		unsigned int spsr = PSR_T|PROCESSOR_MODE_USR;
		/** The IRQ handler return address, where to continue the process. */
		unsigned int return_address = 0x1004;
	} __attribute((packed));

	/**
	 * Prints the contents of a `register_contents` structure to a stream.
	 * This may be used for debugging purposes.
	 * @param output the output stream.
	 * @param registers the register contents to print.
	 * @return the output stream.
	 */
	kostream & operator<<(kostream & output, const register_contents & registers);
}

#endif

