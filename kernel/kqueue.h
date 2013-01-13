// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KQUEUE_H
#define WAGTAIL_KQUEUE_H

namespace wagtail
{
	/**
	 * A quite standard queue implementation. It allows inserting and removing
	 * elements from both the front and the back of the queue.
	 * @tparam T type of data stored in the queue.
	 */
	template<typename T> class kqueue
	{
		public:
			/**
			 * Pushes an element into the front of the queue.
			 * @param data the data element.
			 */
			void push_front(const T & data)
			{
				node * new_node = new node(data, front);

				if(front != nullptr)
					front->set_prev(new_node);
				else
					back = new_node;
				front = new_node;

				++length;
			}

			/**
			 * Pushes an element into the back of the queue.
			 * @param data the data element.
			 */
			void push_back(const T & data)
			{
				node * new_node = new node(data, nullptr, back);

				if(back != nullptr)
					back->set_next(new_node);
				else
					front = new_node;
				back = new_node;

				++length;
			}

			/**
			 * Pops an element from the back of the queue.
			 * @param retval variable to store the popped off element in.
			 * @return `true` if an element was popped off the stack, `false` otherwise.
			 */
			bool pop_back(T & retval)
			{
				if(back == nullptr)
					return false;
				else {
					node * n = back;
					retval = n->get_data();

					if(n->get_prev() != nullptr)
					{
						back = n->get_prev();
						back->set_next(nullptr);
					} else {
						back = nullptr;
						front = nullptr;
					}

					--length;
					delete n;
					return true;
				}
			}

			/**
			 * Pops an element from the front of the queue.
			 * @param retval variable to store the popped off element in.
			 * @return `true` if an element was popped off the stack, `false` otherwise.
			 */
			bool pop_front(T & retval)
			{
				if(front == nullptr)
					return false;
				else {
					node * n = front;
					retval = n->get_data();

					if(n->get_next() != nullptr)
					{
						front = n->get_next();
						front->set_prev(nullptr);
					} else {
						back = nullptr;
						front = nullptr;
					}

					--length;
					delete n;
					return true;
				}
			}

			/**
			 * Gets the queue length.
			 * @return the length of the queue.
			 */
			unsigned int get_length() const { return length; }

		private:
			class node
			{
				public:
					node(const T & data, node * next = nullptr, node * prev = nullptr)
						: data(data), next(next), prev(prev) {}

					T & get_data() { return data; }

					node * get_next() { return next; }
					void set_next(node * next) { this->next = next; }
					node * get_prev() { return prev; }
					void set_prev(node * prev) { this->prev = prev; }
				private:
					T data;
					node * next, * prev;
			};
		public:
			/**
			 * Iterator class, for use with ranged for loops.
			 */
			class iterator
			{
				friend class wagtail::kqueue<T>;

				public:
					bool operator==(const iterator & i) const { return n == i.n; }
					bool operator!=(const iterator & i) const { return !(*this == i); }
					const T & operator*() const { return n->get_data(); }
					iterator & operator++() { n = n->get_next(); return *this; }
				private:
					node * n = nullptr;
			};

			/**
			 * Gets an iterator pointing to the first element in the queue.
			 * @return an iterator pointing to the first element in the queue.
			 */
			iterator begin() { iterator retval; retval.n = front; return retval; }
			/**
			 * Gets an iterator pointing to the element after the last element in the queue.
			 * @return an iterator pointing at the element after the last element in the queue.
			 */
			iterator end() { iterator retval; return retval; }

		private:
			node * front = nullptr, * back = nullptr;
			int length = 0;
	};
}

#endif

