WagtailOS README
================

Introduction
------------

WagtailOS is a simple operating system for the [BeagleBoard](http://www.beagleboard.org) written in C++. It is licensed under the Modified BSD license, which can be found in the COPYING.md file. Source code and bug tracking is available on [GitHub](http://github.com/skordal/wagtail).

The goal of Wagtail is to be a simple operating system for the BeagleBoard, exploiting various features of C++11 (such as `std::function`, which works wonderfully for callback functions) in a way that benefits operating system development. Portability between compilers may be possible, but do not count on it.

How to build Wagtail
--------------------

To build Wagtail, you need the following:
* An arm-none-eabi toolchain with C++11 support
* `mkimage` from the uboot project and `dtc` to build the boot image
* `doxygen` with C++11 support to build the source reference documentation
* Standard utilities such as `grep`, `gzip` and file utilities

To build, simply run `make`. Most build settings can be changed by editing the `config.mk` file in the top of the source tree. The finished kernel image is named `wagtail.itb` and is located in the `kernel` directory. If you do not wish to use the new uBoot format, a regular `uImage` file is also created for your convenience.

If you do choose to use the new image format to boot Wagtail, you will need a uboot image on your Beagleboard that has been patched to enable it. This can easily be done by adding the following somewhere in `include/configs/omap3_beagle.h`:

```C
#define CONFIG_FIT
```

After that, recompile and install U-Boot as instructed by its documentation.

