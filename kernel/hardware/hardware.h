// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_HARDWARE_H
#define WAGTAIL_HARDWARE_H

// Starts the register definitions for a hardware module:
#define BEGIN_MODULE(name) \
	namespace wagtail { namespace hardware { namespace name { struct registers {
#define END_MODULE() };}}}

// Defines a hardware register:
#define REGISTER(name, register_offset) \
	struct name { static const unsigned int offset = register_offset; };
// Starts the definition of a hardware register:
#define REGISTER_BEGIN(name, register_offset) \
	struct name { static const unsigned int offset = register_offset;
#define REGISTER_END() };

// Defines the name of a bit in a 8-bit hardware register:
#define BITNAME_8(name, num) \
	static const unsigned char name = 1 << num;
// Defines the name of a bit in a 16-bit hardware register:
#define BITNAME_16(name, num) \
	static const unsigned short name = 1 << num;
// Defines the name of a bit in a hardware register:
#define BITNAME(name, num) \
	static const unsigned int name = 1 << num;

// Defines a bitfield of the specified width, starting at the specified bit;
// this macro is meant to be used to shift data into the correct position, not
// to set or clear bits like the bitname macros.
#define BITFIELD(name, num, width) \
	static const unsigned int name = num;

#endif

