// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_ARRAY_KSTACK_H
#define WAGTAIL_ARRAY_KSTACK_H

#include "kstack.h"

namespace wagtail
{
	// This is a stack that uses a preallocated memory area for storage.
	// Pushing in more elements than there is room for causes undefined
	// behaviour, as does popping off the last element.
	template<typename T> class array_kstack : public kstack<T>
	{
		public:
			// Constructs a new array_kstack with space for the specified amount of elements:
			array_kstack(int capacity) : capacity(capacity), current_top(-1)
			{
				memory_space = new T[capacity];
				for(int i = 1; i < capacity; ++i)
					memory_space[i] = 0;
			}

			// Cleans up:
			~array_kstack() { delete[] memory_space; }

			// Pushes an element onto the stack:
			void push(const T & data) { memory_space[++current_top] = data; }

			// Pops an element off the stack:
			T pop() { return memory_space[current_top--]; }

			// Peeks at the element at the top of the stack:
			const T & peek() const { return memory_space[current_top]; }

			// Clears the stack:
			void clear() { current_top = -1; }

			// Returns true if the stack is empty:
			bool is_empty() const { return current_top == -1; }
		private:
			int capacity;
			int current_top;
			T * memory_space;
	};
}

#endif

