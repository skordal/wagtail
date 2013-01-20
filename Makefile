# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Include various variables from config.mk. Edit that file to set build
# tools and variables.
include config.mk

.PHONY: all apps clean clean-docs docs kernel library

all: apps library kernel

clean:
	@$(MAKE) -C kernel clean
	@$(MAKE) -C apps clean
	@$(MAKE) -C library clean

clean-docs:
	@-$(RM) -r docs/html-reference

apps: library
	@$(ECHO) "Building applications..."
	@$(MAKE) -C apps

docs:
	@$(ECHO) "Building reference documentation..."
	@$(MKDIR) -p docs
	@$(DOXYGEN)

kernel:
	@$(ECHO) "Building kernel..."
	@$(MAKE) -C kernel

library:
	@$(ECHO) "Building C library..."
	@$(MAKE) -C library

