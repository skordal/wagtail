WagtailOS README
================

Introduction
------------

WagtailOS is a simple operating system for the [BeagleBoard](http://www.beagleboard.org) written in C++. Source code and bug tracking is available on [GitHub](http://github.com/skordal/wagtail).

Wagtail is written by Kristian Klomsten Skordal. It is licensed under the Modified BSD license, which can be found in the COPYING.md file.

Goals
-----

The goal of Wagtail is to be an exceedingly simple operating system. The most immediate goal is to get the operating system to load and execute applications.

Another goal of Wagtail is to exploit various C++ features in a way that benefits operating system development. Portability is not an issue, neither across processor architectures nor compilers.

Current status
--------------

**This is a work in heavy development, and cannot be used for any serious purpose yet.**

Wagtail currently has a working *memory manager* and *page allocator*. Though inefficient, the memory manager allows the kernel to use _new_ and _delete_ like normal C++ applications.

A device manager is also being written, and will be a part in a system for handling devices like UNIX, with files corresponding to devices.

