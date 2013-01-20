// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_PROCESS_BLOCK_H
#define WAGTAIL_PROCESS_BLOCK_H

namespace wagtail
{
	/**
	 * Base class for all blocking condition classes. These classes are
	 * used to maintain information about why a process is blocking and
	 * when it can continue running.
	 */
	class process_block
	{
		public:
			virtual ~process_block() {}

			/**
			 * Checks if the block is currently blocking.
			 * @return `true` if the process block is active, `false` otherwise.
			 */
			virtual bool is_active() const = 0;

			/**
			 * Sets up return values and writes to process memory.
			 * This is called when a process is scheduled to run after
			 * a block has been released.
			 */
			virtual void block_return() = 0;
	};
}

#endif

