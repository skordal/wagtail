// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_IO_H
#define WAGTAIL_IO_H

namespace wagtail
{
	/**
	 * I/O operations for use with hardware registers.
	 */
	namespace io
	{
		/**
		 * Reads from a register.
		 * @tparam T type of the data to read.
		 * @param base the base address of the hardware module.
		 * @param offset the offset of the register to read from.
		 * @return the value of the register.
		 */
		template<typename T>
		inline T read(const void * base, unsigned int offset = 0)
			{ return *(((volatile T *)((unsigned int) base + offset))); }
		
		/**
		 * Writes to a register.
		 * @tparam T type of the data to write.
		 * @param data the data to write to the register.
		 * @param base the base address of the hardware module.
		 * @param offset the offset of the register to write to.
		 */
		template<typename T>
		inline void write(T data, const void * base, unsigned int offset = 0)
			{ *((volatile T *) ((unsigned int) base + offset)) = data; }

		/**
		 * Reads a value from a register, ORs it with a value, and writes it back.
		 * @tparam T type of the data to operate on.
		 * @param data the data to OR the register value with.
		 * @param base the base address of the hardware module.
		 * @param offset the offset of the register to operate on.
		 */
		template<typename T>
		inline void or_register(T data, const void * base, unsigned int offset = 0)
			{ T temp = read<T>(base, offset); temp |= data; write(temp, base, offset); }

		/**
		 * Reads a value from a register, ANDs it with a value, and writes it back.
		 * @tparam T type of the data to operate on.
		 * @param data the data to AND the register value with.
		 * @param base the base address of the hardware module.
		 * @param offset the offset of the register to operate on.
		 */
		template<typename T>
		inline void and_register(T data, const void * base, unsigned int offset = 0)
			{ T temp = read<T>(base, offset); temp &= data; write(temp, base, offset); }
	}
}

#endif

