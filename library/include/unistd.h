// Wagtail OS C Library
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef __WAGTAIL_LIBC_UNISTD_H__
#define __WAGTAIL_LIBC_UNISTD_H__

#include <sys/types.h>

/**
 * Terminates a process without calling `atexit` functions and flushing open streams.
 * @param status the application status code.
 */
void __attribute((noreturn)) _exit(int status);

#endif

