WagtailOS README
================

Introduction
------------

WagtailOS is a simple operating system for the [BeagleBoard](http://www.beagleboard.org) written in C++. It is licensed under the Modified BSD license, which can be found in the COPYING.md file. Source code and bug tracking is available on [GitHub](http://github.com/skordal/wagtail).

The goal of Wagtail is to be a simple operating system for the BeagleBoard, exploiting various features of C++11 (such as `std::function`, which works wonderfully for callback functions) in a way that benefits operating system development. Portability between compilers may be possible, but do not count on it.

Wagtail currently has a working (though simple and inefficient) memory manager, can read FAT32 partitions on SD cards and it can run the `hello` application from the apps folder (which does nothing, except call SVC 1), and it can possibly fail horribly if said application is not located on the first partition of the SD card.

The scheduler and process code is, however, very much a work in progress and is evolving quickly.

How to build Wagtail
--------------------

To build Wagtail, you need the following:
* An arm-none-eabi toolchain with C++11 support
* `mkimage` from the uboot project and `dtc` to build the boot image
* `doxygen` with C++11 support to build the source reference documentation
* Standard utilities such as `grep`, `gzip` and file utilities

To build, simply run `make`. Most build settings can be changed by editing the `config.mk` file in the top of the source tree.

The finished kernel image is named `wagtail.itb` and is located in the `kernel` directory.

Note that to boot Wagtail, you will need a uboot image on your Beagleboard that has been patched to enable the new FIT image format. This can easily be done by adding the following somewhere in `include/configs/omap3_beagle.h`:

```C
#define CONFIG_FIT
```

After that, recompile and install U-Boot as instructed by its documentation.

