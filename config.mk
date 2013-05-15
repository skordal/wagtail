# Makefile configuration for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Architecture dependent build tools:
TOOL_PREFIX    ?= arm-none-eabi-
TARGET_CC      := $(TOOL_PREFIX)gcc
TARGET_CXX     := $(TOOL_PREFIX)g++
TARGET_LD      := $(TOOL_PREFIX)ld
TARGET_OBJCOPY := $(TOOL_PREFIX)objcopy
TARGET_AR      := $(TOOL_PREFIX)ar

# Build utilities:
DOXYGEN ?= doxygen
ECHO    ?= echo
GREP    ?= grep
GZIP    ?= gzip
LN      ?= ln
MAKE    ?= make
MKDIR   ?= mkdir
MKIMAGE ?= mkimage
TEST    ?= test
RM      ?= rm

# Build flags:
KERNEL_CXXFLAGS  += -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon \
	-ffreestanding -Wa,-mcpu=cortex-a8+sec -Wall -mthumb \
	-fno-builtin -fno-rtti -fno-exceptions -falign-functions=4 \
	-std=gnu++0x -fno-use-cxa-atexit -mthumb-interwork
KERNEL_LDFLAGS += -nostdlib

# Build flags for Wagtail applications:
APP_CXXFLAGS += -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon \
	-nostdinc -Wall -mthumb -fno-builtin -fno-rtti -fno-exceptions \
	-ffreestanding -std=gnu++0x -fno-use-cxa-atexit -mthumb-interwork \
	-isystem ../../library/include/
APP_CFLAGS += -O2 -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon \
	-nostdinc -Wall -mthumb -fno-builtin -std=gnu99 -mthumb-interwork \
	-Werror=implicit-function-declaration -isystem ../../library/include/
APP_LDFLAGS += $(KERNEL_LDFLAGS) -T ../application.ld ../start.o \
	-L ../../library -Wl,--as-needed -lc

# Build flags for the C library:
LIBC_CFLAGS += -O2 -mthumb -mfpu=neon -mcpu=cortex-a8 -mtune=cortex-a8 \
	-ffreestanding -Wa,-mcpu=cortex-a8+sec -Wall -mthumb \
	-nostdinc -falign-functions=4 -std=gnu99 -mthumb-interwork \
	-Werror=implicit-function-declaration -isystem include/
LIBC_LDFLAGS += -nostdlib

