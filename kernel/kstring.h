// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTRING_H
#define WAGTAIL_KSTRING_H

#include "utils.h"

namespace wagtail
{
	/**
	 * A simple string class to simplify working with strings.
	 * @todo Add UTF-8 support.
	 */
	class kstring
	{
		public:
			/** Constant representing an invalid position in a string. */
			static const int npos = -1;

			/**
			 * Constructs a string from a C-string.
			 * @param string the C string to use as initial value.
			 */
			kstring(const char * string);
			/**
			 * Constructs a string by copying the value of another.
			 * @param string the string that should be copied.
			 */
			kstring(const kstring & string);
			/**
			 * Constructs a string by copying part of another string.
			 * @param string the string to copy from.
			 * @param start_offset the offset into `string` to start copying from.
			 * @param end_offset the offset into `string` to copy until.
			 */
			kstring(const kstring & string, int start_offset, int end_offset);
			/** Constructs an empty string object. */
			kstring();

			/** Destructs the string object and frees its buffer. */
			~kstring();

			/** Clears the string, setting its value to "". */
			void clear() { delete[] buffer; buffer = new char[1]; buffer[0] = 0; }

			/**
			 * Appends a C-string to the end of this string.
			 * @param string the string to append.
			 */
			void append(const char * string);
			/**
			 * Appends a string to the end of thsi string.
			 * @param string the string to append.
			 */
			void append(const kstring & string);

			/**
			 * Prepends a C-string to the beginning of this string.
			 * @param string the string to prepend.
			 */
			void prepend(const char * string);
			/**
			 * Prepends a string to the beginning of this string.
			 * @param string the string to prepend.
			 */
			void prepend(const kstring & string);

			/** Converts the entire string to lowercase. */
			void tolower();
			/** Converts the entire string to uppercase. */
			void toupper();

			/**
			 * Gets the directory component of a pathname.
			 * @return the directory component of a pathname.
			 */
			kstring dirname() const;
			/**
			 * Gets the file component of a pathname.
			 * @return the file component of a pathname.
			 */
			kstring filename() const;

			/**
			 * Returns the index of the first occurrence of a character.
			 * @param c the character to look for.
			 * @return the index of the first occurrence of the character, or `kstring::npos`
			 *         if the character was not found.
			 */
			int index(char c) const;

			/**
			 * Returns the index of the last occurrence of a character.
			 * @param c the character to look for.
			 * @return the index of the last occurrence of the character, or `kstring::npos`
			 *         if the character was not found.
			 */
			int rindex(char c) const;

			/**
			 * Assigns the value of a C-string to a string.
			 * @param string the string to copy.
			 */
			kstring & operator=(const char * string);
			/**
			 * Assigns the value of another string to a string.
			 * @param string the string to copy.
			 */
			kstring & operator=(const kstring & string);

			/**
			 * Appends a C-string onto a string.
			 * @param string the string to append.
			 */
			kstring & operator+=(const char * string);
			/**
			 * Appends a string to another string.
			 * @param string the string to append.
			 */
			kstring & operator+=(const kstring & string);
			/**
			 * Appends a single character to a string.
			 * @param c the character to append.
			 */
			kstring & operator+=(char c);

			/**
			 * Compares a string to another for equality.
			 * @param string the string to compare with.
			 */
			bool operator==(const kstring & string) const;
			/**
			 * Compares a string to another for inequality.
			 * @param string the string to compare with.
			 */
			bool operator!=(const kstring & string) const { return !(*this == string); }

			/**
			 * Gets a character from the string.
			 * @param n the index into the string.
			 */
			char & operator[](int n) { return buffer[n]; }
			/**
			 * Gets a character from the string.
			 * @param n the index into the string.
			 */
			char operator[](int n) const { return buffer[n]; }

			/**
			 * Gets the length of a string.
			 * @return the length of a string.
			 */
			unsigned int get_length() const { return utils::strlen(buffer); }

			/**
			 * Checks if a string is empty.
			 * @return `true` if the length of the string is 0, `false` otherwise.
			 */
			bool is_empty() const { return get_length() == 0; }

			/**
			 * Gets the buffer where the string data is stored.
			 * @return the buffer where the string data is stored.
			 */
			const char * get_buffer() const { return buffer; }
		private:
			char * buffer;
	};
}

#endif

