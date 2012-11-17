WagtailOS README
================

Introduction
------------

WagtailOS is a simple operating system for the [BeagleBoard](http://www.beagleboard.org) written in C++. It is licensed under the Modified BSD license, which can be found in the COPYING.md file. Source code and bug tracking is available on [GitHub](http://github.com/skordal/wagtail).

The goal of Wagtail is to be a simple operating system for the BeagleBoard, exploiting various C++ features in a way that benefits operating system development. Portability between compilers is not a goal.

Another goal of Wagtail is to exploit various C++ features in a way that benefits operating system development.

Wagtail currently has working (though perhaps a bit simple and inefficient) memory management, can read from SD cards, and can list directory contents on FAT32 partitions on the aforementioned SD card.

How to build Wagtail
--------------------

To build Wagtail, you need the following:
* An arm-none-eabi toolchain
* `mkimage` from the uboot project
* `dtc`, also to create the boot image

When you have the pre-requisites, simply run `make`.

Note that to boot Wagtail, you will need a uboot image on your Beagleboard that has been patched to enable the new FIT image format. This can easily be done by adding the following somewhere in include/configs/omap3_beagle.h:

```C
#define CONFIG_FIT
```

After that, recompile uboot and copy u-boot.bin to your Beagleboard SD card.

