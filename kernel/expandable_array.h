// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_EXPANDABLE_ARRAY_H
#define WAGTAIL_EXPANDABLE_ARRAY_H

namespace wagtail
{
	// An expandable array class. It automatically expands when it is asked
	// to store an element at a position outside its capacity. This array
	// will need to be initialized just like other arrays.
	template<typename T> class expandable_array final
	{
		public:
			// Creates a new expandable array with the specified capacity, which increases
			// with the specified amount if an element is written to the array at the index
			// just above the current capacity.
			expandable_array(int initial_capacity, int expand = 1)
				: capacity(capacity), expand_amount(expand), array(new T[initial_capacity]) {}
			~expandable_array() { delete[] array; }

			T & get(int index)
			{
				if(index >= capacity && index < capacity + expand_amount)
					expand();
				else if(index >= capacity)
					expand(index);

				return array[index];
			}

			void put(const T & data, int index)
			{
				if(index >= capacity && index < capacity + expand_amount)
					expand();
				else if(index >= capacity)
					expand(index);

				return array[index];
			}

			T & operator[](int index) { return get(index); }
			int get_capacity() const { return capacity; }

			void expand(int new_capacity = -1)
			{
				if(new_capacity == -1)
					new_capacity = capacity + expand_amount;

				T * new_array = new T[new_capacity];
				for(int i = 0; i < capacity; ++i)
					new_array[i] = array[i];
				capacity = new_capacity;

				delete[] array;
				array = new_array;
			}
		private:
			int capacity;
			int expand_amount;

			T * array;
	};
}

#endif

