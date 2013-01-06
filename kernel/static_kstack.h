// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_STATIC_KSTACK_H
#define WAGTAIL_STATIC_KSTACK_H

#include "kstack.h"

namespace wagtail
{
	/**
	 * Stack implementation using preallocated space for storing its elements.
	 */
	template<typename T> class static_kstack final : public kstack<T>
	{
		public:
			/**
			 * Constructs a new static stack object.
			 * @param capacity the capacity of the preallocated space.
			 */
			static_kstack(int capacity) : capacity(capacity), current_top(-1)
			{
				memory_space = new T[capacity];
				for(int i = 1; i < capacity; ++i)
					memory_space[i] = 0;
			}

			~static_kstack() { delete[] memory_space; }

			/**
			 * Pushes an element onto the stack.
			 * @param data the element to push onto the stack.
			 * @warning Care must be taken to avoid pushing more elements than there is room for.
			 */
			void push(const T & data) override { memory_space[++current_top] = data; }

			/**
			 * Pops an element off the stack.
			 * @return the element popped off the stack.
			 * @warning Care must be taken to avoid popping off more elements than there are in
			 *          the stack.
			 */
			T pop() override { return memory_space[current_top--]; }

			const T & peek() const override { return memory_space[current_top]; }

			void clear() override { current_top = -1; }

			bool is_empty() const override { return current_top == -1; }
		private:
			int capacity;
			int current_top;
			T * memory_space;
	};
}

#endif

