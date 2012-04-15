// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KLIST_H
#define WAGTAIL_KLIST_H

namespace wagtail
{
	// Singly-linked list type. There is no support for searching for
	// or removing elements in a list at the moment.
	template<typename T> class klist
	{
		public:
			klist() : first(0), last(0), current(0) {}

			// Appends an item to the end of the list:
			void append(const T & item)
			{
				node * new_node = new node(item);

				if(last == 0)
				{
					last = new_node;
					first = last;
				} else {
					last->next = new_node;
					last = new_node;
				}
			}

			// Rewinds the internal list pointer:
			void rewind() { current = first; }
			// Goes to the next list position if possible, or returns
			// false if the list is already at the end:
			bool next()
			{
				if(current->next)
				{
					current = current->next;
					return true;
				} else 
					return false;
			}

			// Returns true if the list is at the end:
			bool is_at_end() const { return current == 0; }
			// Returns true if the list is empty:
			bool is_empty() const { return first == 0; }

			// Gets the data at the current list position:
			const T & get_data() const { return current->data; }
			T & get_data() { return current->data; }
		private:
			struct node
			{
				node(const T & data) : data(data), next(0) {}
				T data;
				node * next;
			};

			// Node pointers:
			node * first, * last, * current;
	};
}

#endif

