// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_STATIC_KSTACK_H
#define WAGTAIL_STATIC_KSTACK_H

#include "kstack.h"

namespace wagtail
{
	// This is a stack that uses a preallocated memory area for storage.
	// Pushing in more elements than there is room for causes undefined
	// behaviour, as does popping off the last element.
	template<typename T> class static_kstack final : public kstack<T>
	{
		public:
			// Constructs a new static_kstack with enough space for the specified
			// amount of elements:
			static_kstack(int capacity) : capacity(capacity), current_top(-1)
			{
				memory_space = new T[capacity];
				for(int i = 1; i < capacity; ++i)
					memory_space[i] = 0;
			}

			// Frees the allocated space:
			~static_kstack() { delete[] memory_space; }

			// Pushes an element onto the stack:
			void push(const T & data) override { memory_space[++current_top] = data; }

			// Pops an element off the stack:
			T pop() override{ return memory_space[current_top--]; }

			// Peeks at the element at the top of the stack:
			const T & peek() const override { return memory_space[current_top]; }

			// Clears the stack:
			void clear() override { current_top = -1; }

			// Returns true if the stack is empty:
			bool is_empty() const override { return current_top == -1; }
		private:
			int capacity;
			int current_top;
			T * memory_space;
	};
}

#endif

