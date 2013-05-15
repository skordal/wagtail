# Makefile for the Wagtail OS Hello World application
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

include ../../config.mk
.PHONY: all clean

all: main.cpp
	$(TARGET_CXX) $(APP_CXXFLAGS) -o hello.elf main.cpp $(APP_LDFLAGS)
	$(TARGET_OBJCOPY) -S -O binary -j .text -j .data hello.elf hello

clean:
	-$(RM) hello.elf hello

