# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Include various variables from config.mk. Edit that file to set build
# tools and variables.
include config.mk

.PHONY: all kernel apps clean

all: kernel apps

kernel:
	@echo "Building kernel..."
	@make -C kernel

apps:
	@echo "Building applications..."
	@make -C apps

clean:
	@make -C kernel clean
	@make -C apps clean

