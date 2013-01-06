// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_KSTREAM_H
#define WAGTAIL_KSTREAM_H

#include "kstring.h"

namespace wagtail
{
	/**
	 * Output stream class.
	 */
	class kostream
	{
		public:
			/**
			 * Puts a character into the stream. To use the `kostream` class,
			 * only this method needs to be overridden.
			 * @param character the carachter to put into the stream.
			 */
			virtual kostream & operator << (char character) = 0;
			/**
			 * Puts a string into the stream.
			 * @param string the string to put into the stream.
			 */
			virtual kostream & operator << (const char * string);
			/**
			 * Puts a `kstring` into the stream.
			 * @param string the `kstring` to put into the stream.
			 */
			virtual kostream & operator << (const kstring & string);
			/**
			 * Puts the hexadecimal value of a pointer into the stream.
			 * @param pointer the pointer to put into the stream.
			 */
			virtual kostream & operator << (void * pointer);
			/**
			 * Puts the base-10 representation of an integer into the stream.
			 * @param value the value to put into the stream.
			 */
			virtual kostream & operator << (int value);
			/**
			 * Puts the value of a boolean into the stream. This is done
			 * by printing either `true` or `false` corresponding to the
			 * value of `boolean`.
			 * @param boolean the value to put into the stream.
			 */
			virtual kostream & operator << (bool boolean);
	};

	/**
	 * Input stream class.
	 */
	class kistream
	{
		public:
			/**
			 * Reads a character from the stream. To use this class, only this operator
			 * needs to be overridden.
			 * @param character the character read from the stream.
			 */
			virtual kistream & operator >> (char & character) = 0;
			/**
			 * Reads a string from the stream. The reading stops when the character
			 * previously set as deliminator has been read.
			 * @param string the output string.
			 */
			virtual kistream & operator >> (const char * string);

			/**
			 * Sets the deliminator for use when reading strings.
			 * @param c the character to use as deliminator.
			 */
			void set_deliminator(char c) { delim = c; }
			/**
			 * Gets the deliminator that is used when reading string.
			 * @return the character used as deliminator.
			 */
			char get_deliminator() const { return delim; }
		protected:
			/**
			 * Constructs a new input stream object.
			 * @param delim the character to use as string deliminator.
			 */
			kistream(char delim = '\n') : delim(delim) {}
		private:
			char delim;
	};

	/**
	 * Constants to use with kernel streams.
	 */
	namespace kstream
	{
		/** Newline constant. */
		extern const char * newline;
	}
}

#endif

