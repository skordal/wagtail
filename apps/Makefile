# Makefile for the Wagtail OS
# (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
# Report bugs and issues on <http://github.com/skordal/wagtail/issues>

# Build configuration file:
include ../config.mk

.PHONY: all clean

# List of applications (subdirectories) to build:
APPLICATIONS := hello
.PHONY: $(APPLICATIONS)

# Main build target:
all: header.o start.o $(APPLICATIONS)

# Recipie for building the application header:
header.o: header.S
	$(CXX) $(CXXFLAGS) -c -o header.o header.S

# Recipie for the application start file:
start.o: start.S
	$(CXX) $(CXXFLAGS) -c -o start.o start.S

# Recipie for building application directories:
$(APPLICATIONS): header.o start.o application.ld
	@$(MAKE) -C $@

# Clean recipie:
clean:
	-$(RM) header.o start.o
	@for dir in $(APPLICATIONS); do \
		$(MAKE) -C $$dir clean; \
	done

