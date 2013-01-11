# Makefile configuration for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Architecture dependent build tools:
TOOL_PREFIX ?= arm-none-eabi-
CXX          = $(TOOL_PREFIX)gcc
LD           = $(TOOL_PREFIX)ld
OBJCOPY      = $(TOOL_PREFIX)objcopy

# Build utilities:
DOXYGEN ?= doxygen
GREP    ?= grep
GZIP    ?= gzip
MKDIR   ?= mkdir
MKIMAGE ?= mkimage
RM      ?= rm

# Build flags:
KERNEL_CXXFLAGS  += -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon \
	-ffreestanding -Wa,-mcpu=cortex-a8+sec -Wall -mthumb \
	-fno-builtin -fno-rtti -fno-exceptions -falign-functions=4 \
	-std=gnu++0x -fno-use-cxa-atexit -mthumb-interwork
KERNEL_LDFLAGS += -nostdlib

# Build flags for Wagtail applications:
APP_CXXFLAGS += $(KERNEL_CXXFLAGS)
APP_LDFLAGS += $(KERNEL_LDFLAGS) -T ../application.ld ../start.o

