// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_CXX_SUPPORT_H
#define WAGTAIL_CXX_SUPPORT_H

#include "kernel.h"

/** Function required to use pure virtual methods. */
extern "C" void __cxa_pure_virtual();

/**
 * Memory copying function. This is sometimes emitted by the compiler when
 * default constructors are constructed.
 * @param dest destination.
 * @param src source.
 * @param n size of the area to copy.
 * @return a pointer to the destination.
 */
extern "C" void * memcpy(void * dest, void * src, unsigned int n);

/**
 * Memory fill function. This is sometimes emitted by the compiler when
 * default constructors are construced.
 * @param s start of the area to fill.
 * @param c the byte to fill the area with.
 * @param n the size of the area to fill.
 * @return a pointer to the destination area.
 */
extern "C" void * memset(void * d, char c, unsigned int n);

#endif

