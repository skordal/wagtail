WagtailOS README
================

Introduction
------------

WagtailOS is a simple operating system for the [BeagleBoard](http://www.beagleboard.org) written in C++. It is licensed under the Modified BSD license, which can be found in the COPYING.md file. Source code and bug tracking is available on [GitHub](http://github.com/skordal/wagtail).

Goals
-----

The goal of Wagtail is to be a simple operating system for the BeagleBoard, exploiting various C++ features in a way that benefits operating system development. Portability between compilers is not a goal.
The goal of Wagtail is to be an exceedingly simple operating system. The most immediate goal is to get the operating system to load and execute applications.

Another goal of Wagtail is to exploit various C++ features in a way that benefits operating system development. Portability is not a goal, neither across processor architectures nor compilers.

Current status
--------------

**Wagtail is under heavy development, and cannot be used for anything serious yet.**

Wagtail currently has a working memory manager and page allocator. Though inefficient at the moment, the memory manager allows the kernel to use the _new_ and _delete_ operators just like normal C++ applications.

