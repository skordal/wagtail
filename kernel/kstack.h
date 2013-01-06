// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTACK_H
#define WAGTAIL_KSTACK_H

namespace wagtail
{
	/**
	 * Simple stack interface, for use in stack implementations.
	 */
	template<typename T> class kstack
	{
		public:
			/** Base stack destructor. */
			virtual ~kstack() {}

			/**
			 * Pushes an element onto a stack.
			 * @param data the element to push onto the stack.
			 */
			virtual void push(const T & data) = 0;

			/**
			 * Pops an object off the stack.
			 * @return the object popped off the stack.
			 */
			virtual T pop() = 0;

			/**
			 * Pops an object off the stack.
			 * @param var the object popped off the stack is returned through this parameter.
			 */
			virtual void pop(T & var) { var = pop(); }

			/**
			 * Returns the object at the top of the stack without popping it off.
			 * @return the object at the top of the stack.
			 */
			virtual const T & peek() const = 0;

			/** Clears the stack by popping all the elements and discarding them. */
			virtual void clear() = 0;

			/**
			 * Checks if the stack is empty.
			 * @return `true` if the stack is empty, `false` otherwise.
			 */
			virtual bool is_empty() const = 0;
	};
}

#endif

