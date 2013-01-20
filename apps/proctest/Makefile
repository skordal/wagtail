# Makefile for the Wagtail OS Process Test Application
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>
include ../../config.mk
.PHONY: all clean

all:
	$(CC) $(APP_CFLAGS) -o proctest.elf proctest.c $(APP_LDFLAGS)
	$(OBJCOPY) -S -O binary -j .text -j .data proctest.elf proctest

clean:
	-$(RM) proctest.elf proctest

