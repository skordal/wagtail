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
		register_contents();
		register_contents(const register_contents & copy);

		/** Registers r0 - r14. */
		unsigned int r[15];
		/** The CPSR of the process. */
		unsigned int spsr;
		/** The IRQ handler return address, where to continue the process. */
		unsigned int return_address;
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

