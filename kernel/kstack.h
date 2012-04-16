// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTACK_H
#define WAGTAIL_KSTACK_H

namespace wagtail
{
	// Stack interface, used as base class for stack implementations:
	template<typename T> class kstack
	{
		public:
			virtual ~kstack() {}

			// Pushes an object onto the stack:
			virtual void push(const T & data) = 0;

			// Pops an object off the stack:
			virtual T pop() = 0;

			// Pops an object off the stack and returns it trough the argument:
			virtual void pop(T & var) { var = pop(); }

			// Peeks at the object at the top of the stack:
			virtual const T & peek() const = 0;

			// Pops all items off the stack and discards them:
			virtual void clear() = 0;

			// Returns true if the stack is empty:
			virtual bool is_empty() const = 0;
	};
}

#endif

