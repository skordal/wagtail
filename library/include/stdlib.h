// Wagtail OS C Library
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef __WAGTAIL_LIBC_STDLIB_H
#define __WAGTAIL_LIBC_STDLIB_H

/**
 * Terminates a process, calling `atexit` functions and flushing open streams.
 * @param status the application status code.
 */
void __attribute((noreturn)) exit(int status);

/**
 * Terminates a process without calling `atexit` functions and flushing open streams.
 * @param status the application status code.
 */
void __attribute((noreturn)) _Exit(int status);

#endif

