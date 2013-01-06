# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Include various variables from config.mk. Edit that file to set build
# tools and variables.
include config.mk

.PHONY: all apps clean docs kernel 

all: kernel

clean:
	@make -C kernel clean
	@make -C apps clean

apps:
	@echo "Building applications..."
	@make -C apps

docs:
	@echo "Building reference documentation..."
	@$(MKDIR) -p docs
	@$(DOXYGEN)

kernel:
	@echo "Building kernel..."
	@make -C kernel

