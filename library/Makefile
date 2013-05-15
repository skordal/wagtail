# Makefile for the Wagtail OS C library
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>
include ../config.mk

.PHONY: all clean prepare

# Additional CFLAGS to accomodate putting the headers in their own directory:
LIBC_CFLAGS += -isystem include/

SOURCE_FILES := \
	exit.c 
ASSEMBLER_FILES := 
OBJECT_FILES = $(ASSEMBLER_FILES:.S=.o) $(SOURCE_FILES:.c=.o)

all: prepare $(OBJECT_FILES)
	$(TARGET_AR) rcs libc.a $(OBJECT_FILES)

clean:
	-$(RM) $(OBJECT_FILES)
	-$(RM) include/wagtail/syscalls.h libc.a

prepare:
	$(MKDIR) -pv include/wagtail
	$(TEST) -L include/wagtail/syscalls.h || $(LN) -s $(shell pwd)/../kernel/asm/syscalls.h include/wagtail/syscalls.h

%.o: %.S
	$(TARGET_CC) -c $(LIBC_CFLAGS) $< -o $@

%.o: %.c
	$(TARGET_CC) -c $(LIBC_CFLAGS) $< -o $@


