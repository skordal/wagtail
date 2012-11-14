# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Include various variables from config.mk. Edit that file to set build
# tools and variables.
include ../config.mk

# Source files to build:
SOURCE_FILES := \
	aborts.cpp \
	clock_mgr.cpp \
	cxx_support.cpp \
	device.cpp \
	device_mgr.cpp \
	direntry.cpp \
	filesystem.cpp \
	filesystems/fat32.cpp \
	irq_handler.cpp \
	kernel.cpp \
	kstream.cpp \
	mm.cpp \
	mmu.cpp \
	partition.cpp \
	scm.cpp \
	sd.cpp \
	uart.cpp \
	utils.cpp

# Assembler source files to build:
ASSEMBLER_FILES := \
	interrupts.S \
	start.S

# List of object files to be built:
OBJECT_FILES = $(ASSEMBLER_FILES:.S=.o) \
	$(SOURCE_FILES:.cpp=.o)

.PHONY: all clean

all: $(OBJECT_FILES)
	$(LD) $(LDFLAGS) -T wagtail.ld -o wagtail.elf $(OBJECT_FILES)
	$(OBJCOPY) -S -x -O binary -j .text -j .data wagtail.elf wagtail.bin
	$(MKIMAGE) $(MKIMAGE_FLAGS) -d wagtail.bin uImage # > /dev/null

clean:
	-$(RM) $(OBJECT_FILES)
	-$(RM) wagtail.elf wagtail.bin uImage

%.o: %.S
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@


