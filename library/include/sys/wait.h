// Wagtail OS C Library
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef __WAGTAIL_LIBC_SYS_WAIT_H__
#define __WAGTAIL_LIBC_SYS_WAIT_H__

#include <sys/types.h>

/**
 * Waits for the termination of any child process.
 * @param stat_loc a pointer to where the return status of the terminating
 *                 child process should be stored.
 * @return the PID of the terminated child process or -1 if an error occurs.
 */
pid_t wait(int * stat_loc);

/**
 * Waits for the termination of a child process.
 * @param pid which child process to wait for, or one of the following:
 *	* Less than -1 to wait for any process whose process group is equal
 *	  to the absolute value of the `pid` argument.
 * 	* -1 to wait for any child process. `waitpid` then works exactly like
 * 	  `wait`.
 * 	* 0 to wait for the termination of any child process in the same process
 * 	  group as the current process.
 * @param stat_loc a pointer to where the return status of the terminating
 *                 child process should be stored.
 * @return the PID of the terminated child process or -1 if an error occurs.
 */
pid_t waitpid(pid_t pid, int * stat_loc, int options);

#endif

