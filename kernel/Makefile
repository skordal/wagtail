# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Include various variables from config.mk. Edit that file to set build
# tools and variables.
include ../config.mk

# Source files to build:
SOURCE_FILES := \
	aborts.cpp \
	cxx_support.cpp \
	irq_handler.cpp \
	kernel.cpp \
	kthread.cpp \
	kstream.cpp \
	kstring.cpp \
	mm.cpp \
	mmu.cpp \
	process.cpp \
	register_contents.cpp \
	scheduler.cpp \
	syscall.cpp \
	timer.cpp \
	uart.cpp \
	utils.cpp

# Assembler source files to build:
ASSEMBLER_FILES := \
	context_switch.S \
	interrupts.S \
	start.S

# List of object files to be built:
OBJECT_FILES = $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.cpp=.o)

.PHONY: all clean

all: $(OBJECT_FILES)
	$(LD) $(KERNEL_LDFLAGS) -T wagtail.ld -o wagtail.elf -Map system.map $(OBJECT_FILES)
	$(OBJCOPY) -S -x -O binary -j .text -j .data wagtail.elf wagtail.bin
	$(GZIP) -c wagtail.bin > wagtail.bin.gz
	$(MKIMAGE) -f wagtail.its wagtail.itb
	$(MKIMAGE) -A arm -O linux -T kernel -C gzip -a 0x81000000 -e 0x81000000 \
		-n WagtailOS -d wagtail.bin.gz uImage

clean:
	-$(RM) $(OBJECT_FILES)
	-$(RM) wagtail.elf wagtail.bin wagtail.bin.gz wagtail.itb system.map

%.o: %.S
	$(CXX) -c $(KERNEL_CXXFLAGS) $< -o $@

%.o: %.cpp %.h
	$(CXX) -c $(KERNEL_CXXFLAGS) $< -o $@


