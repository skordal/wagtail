// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_EXPANDABLE_ARRAY_H
#define WAGTAIL_EXPANDABLE_ARRAY_H

namespace wagtail
{
	/**
	 * An expandable array class. An expandable array works like a normal array,
	 * but automatically expands when asked to store an element at a position which
	 * is above its capacity. 
	 */
	template<typename T> class expandable_array final
	{
		public:
			/**
			 * Constructs an expandable array.
			 * @param initial_capacity the initial capacity of the array.
			 * @param expand the number of elements to expand with when the array is expanding.
			 */
			expandable_array(int initial_capacity, int expand = 1)
				: capacity(capacity), expand_amount(expand), array(new T[initial_capacity]) {}
			/** Destroys the array by deleting the underlying array. */
			~expandable_array() { delete[] array; }

			/**
			 * Gets the element stored at the specified index in the array.
			 * @warning Causes the array to expand if an element is requested above its bounds.
			 * @param index the index of the element to get.
			 * @return the element stored at the specified position in the array.
			 */
			T & get(int index)
			{
				if(index >= capacity && index < capacity + expand_amount)
					expand();
				else if(index >= capacity)
					expand(index);

				return array[index];
			}

			/**
			 * Puts an element into the array.
			 * @param data the data to enter into the array.
			 * @param index the index at which to put the data in the array.
			 */
			void put(const T & data, int index)
			{
				if(index >= capacity && index < capacity + expand_amount)
					expand();
				else if(index >= capacity)
					expand(index);

				return array[index];
			}

			/**
			 * Gets a reference to an element stored in the array.
			 * @param index the index where the element is stored.
			 * @return a reference to the element stored at the specified index.
			 */
			T & operator[](int index) { return get(index); }

			/**
			 * Gets the current capacity of the array.
			 * @return the current capacity of the array.
			 */
			int get_capacity() const { return capacity; }

			/**
			 * Expands the array.
			 * @param new_capacity the capacity of the array after the expansion. `-1` means
			 *                     expand with the preset amount.
			 */
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

