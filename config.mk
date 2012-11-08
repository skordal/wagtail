# Makefile configuration for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Architecture dependent build tools:
TOOL_PREFIX ?= arm-none-eabi-
CXX          = $(TOOL_PREFIX)gcc
LD           = $(TOOL_PREFIX)ld
OBJCOPY      = $(TOOL_PREFIX)objcopy

# Build utilities:
GREP    ?= grep
MKIMAGE ?= mkimage
RM      ?= rm

# Build flags:
CXXFLAGS  += -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon \
	-ffreestanding -Wa,-mcpu=cortex-a8+sec -Wall -mthumb \
	-mno-thumb-interwork -fno-builtin -fno-rtti -fno-exceptions \
	-falign-functions=4 -std=gnu++0x
LDFLAGS += -nostdlib
MKIMAGE_FLAGS  = -A arm -O linux -T kernel -C none -a 0x80008000 \
	-e 0x80008000 -n WagtailOS

# Build flags for Wagtail applications:
APP_CXXFLAGS += $(CXXFLAGS)
APP_LDFLAGS += $(LDFLAGS) -T ../application.ld ../start.o

